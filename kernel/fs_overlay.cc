#include "ext2.h"
#include "debug.h"
#include "libk.h"

// Helper: manual strcmp
static int my_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// Helper: manual string copy
static void my_strcpy(char* dest, const char* src, uint32_t max_len) {
    uint32_t i;
    for (i = 0; i < max_len - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

// FilesystemOverlay implementation

FilesystemOverlay::FilesystemOverlay() 
    : modifications(nullptr), modification_count(0), 
      modification_capacity(0), next_fake_inode(1000000) {
    // Start with space for 32 modifications
    modification_capacity = 32;
    modifications = new FsModification[modification_capacity];
}

FilesystemOverlay::~FilesystemOverlay() {
    if (modifications) {
        delete[] modifications;
    }
}

bool FilesystemOverlay::is_deleted(const char* path) {
    for (uint32_t i = 0; i < modification_count; i++) {
        if (modifications[i].type == FsModification::DELETE) {
            if (my_strcmp(modifications[i].path, path) == 0) {
                return true;
            }
        }
    }
    return false;
}

FsModification* FilesystemOverlay::find_entry(const char* path) {
    // Search from most recent to oldest
    for (int i = modification_count - 1; i >= 0; i--) {
        // Check if this path was created
        if (modifications[i].type == FsModification::CREATE_DIR ||
            modifications[i].type == FsModification::CREATE_FILE) {
            if (my_strcmp(modifications[i].path, path) == 0) {
                return &modifications[i];
            }
        }
        // Check if something was renamed to this path
        if (modifications[i].type == FsModification::RENAME) {
            if (my_strcmp(modifications[i].new_path, path) == 0) {
                return &modifications[i];
            }
        }
    }
    return nullptr;
}

const char* FilesystemOverlay::resolve_path(const char* path, char* buffer) {
    // Follow rename chain
    const char* current_path = path;
    
    for (int i = modification_count - 1; i >= 0; i--) {
        if (modifications[i].type == FsModification::RENAME) {
            // Manual strcmp
            bool match = true;
            for (uint32_t j = 0; ; j++) {
                if (modifications[i].new_path[j] != current_path[j]) {
                    match = false;
                    break;
                }
                if (modifications[i].new_path[j] == '\0') {
                    break;
                }
            }
            if (match) {
                current_path = modifications[i].path;
            }
        }
    }
    
    if (current_path != path) {
        // Manual strcpy
        uint32_t j;
        for (j = 0; current_path[j] != '\0'; j++) {
            buffer[j] = current_path[j];
        }
        buffer[j] = '\0';
        return buffer;
    }
    return path;
}

void FilesystemOverlay::add_modification(const FsModification& mod) {
    // Expand array if needed
    if (modification_count >= modification_capacity) {
        uint32_t new_capacity = modification_capacity * 2;
        FsModification* new_mods = new FsModification[new_capacity];
        
        for (uint32_t i = 0; i < modification_count; i++) {
            new_mods[i] = modifications[i];
        }
        
        delete[] modifications;
        modifications = new_mods;
        modification_capacity = new_capacity;
    }
    
    modifications[modification_count++] = mod;
}

bool FilesystemOverlay::create_directory(const char* path, uint32_t mode) {
    // Check if already exists or deleted
    if (find_entry(path) != nullptr) {
        return false;  // Already exists
    }
    
    FsModification mod;
    mod.type = FsModification::CREATE_DIR;
    my_strcpy(mod.path, path, sizeof(mod.path));
    mod.fake_inode = get_fake_inode();
    mod.mode = mode | 0x4000;  // Directory bit
    mod.size = 4096;  // Default directory size
    
    add_modification(mod);
    return true;
}

bool FilesystemOverlay::delete_entry(const char* path) {
    // Don't add duplicate delete
    if (is_deleted(path)) {
        return false;
    }
    
    FsModification mod;
    mod.type = FsModification::DELETE;
    my_strcpy(mod.path, path, sizeof(mod.path));
    
    add_modification(mod);
    return true;
}

bool FilesystemOverlay::rename_entry(const char* old_path, const char* new_path) {
    FsModification mod;
    mod.type = FsModification::RENAME;
    my_strcpy(mod.path, old_path, sizeof(mod.path));
    my_strcpy(mod.new_path, new_path, sizeof(mod.new_path));
    
    // If old_path was created in overlay, update its path
    FsModification* existing = find_entry(old_path);
    if (existing) {
        existing->fake_inode = get_fake_inode();
        mod.fake_inode = existing->fake_inode;
        mod.mode = existing->mode;
    }
    
    add_modification(mod);
    return true;
}

