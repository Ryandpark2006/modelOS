#pragma once

#include "ide.h"
#include "shared.h"
#include "block_cache.h"

#define EXT2_SUPER_MAGIC 0xEF53
#define EXT2_ROOT_INO 2

#define EXT2_S_IFREG 0x8000  
#define EXT2_S_IFDIR 0x4000  
#define EXT2_S_IFLNK 0xA000  

struct ext2_inode {
    uint16_t i_mode;        
    uint16_t i_uid;         
    uint32_t i_size;        
    uint32_t i_atime;       
    uint32_t i_ctime;       
    uint32_t i_mtime;
    uint32_t i_dtime;       
    uint16_t i_gid;         
    uint16_t i_links_count; 
    uint32_t i_blocks;    
    uint32_t i_flags;       
    uint32_t i_osd1;        
    uint32_t i_block[15];
    uint32_t i_generation;
    uint32_t i_file_acl;
    uint32_t i_dir_acl;
    uint32_t i_faddr;       
    uint8_t  i_osd2[12];    
} __attribute__((packed));   

struct ext2_dir_entry {
    uint32_t inode;         // inode number (0 = unused entry)
    uint16_t rec_len;       
    uint8_t  name_len;
    uint8_t  file_type;     
    char     name[];        
} __attribute__((packed));

class Node : public BlockIO { 
private: 
    StrongPtr<Ide> ide; 
    ext2_inode inode; 
    uint32_t fs_block_size;
    uint32_t inode_table_block;
    BlockCache* cache;  
    
    void read_fs_block(uint32_t fs_block_num, char* buffer); 

public:

    const uint32_t number;

    Node(StrongPtr<Ide> ide, uint32_t inode_num, uint32_t fs_block_size,
         uint32_t inode_size, uint32_t inode_table_block, BlockCache* cache);

    uint32_t size_in_bytes() override {
        return inode.i_size;
    }

    void read_block(uint32_t number, char* buffer) override;

    inline uint16_t get_type() {
        return inode.i_mode;
    }

    bool is_dir() {
        return (inode.i_mode & 0xF000) == EXT2_S_IFDIR;
    }

    bool is_file() {
        return (inode.i_mode & 0xF000) == EXT2_S_IFREG;
    }

    bool is_symlink() {
        return (inode.i_mode & 0xF000) == EXT2_S_IFLNK;
    }

    // If this node is a symbolic link, fill the buffer with
    // the name the link referes to.
    //
    // Panics if the node is not a symbolic link
    //
    // The buffer needs to be at least as big as the the value
    // returned by size_in_byte()
    void get_symbol(char* buffer);

    uint32_t n_links() {
        return inode.i_links_count;
    }

    void show(const char* msg) {
    }

    uint32_t entry_count();
};

// Filesystem modification types for in-memory overlay
struct FsModification {
    enum Type { 
        CREATE_DIR,     // Created directory
        CREATE_FILE,    // Created file
        DELETE,         // Deleted file/directory
        RENAME          // Renamed file/directory
    } type;
    
    char path[256];         // Original path
    char new_path[256];     // New path (for RENAME)
    uint32_t fake_inode;    // Fake inode number for created entries
    uint32_t mode;          // File mode/permissions
    uint32_t size;          // File size (for created files)
    
    FsModification() : type(CREATE_DIR), fake_inode(0), mode(0), size(0) {
        path[0] = '\0';
        new_path[0] = '\0';
    }
};

// Per-process filesystem overlay for write operations
class FilesystemOverlay {
private:
    FsModification* modifications;
    uint32_t modification_count;
    uint32_t modification_capacity;
    uint32_t next_fake_inode;
    
public:
    FilesystemOverlay();
    ~FilesystemOverlay();
    
    // Check if a path has been deleted
    bool is_deleted(const char* path);
    
    // Check if a path exists in the overlay (created or renamed to)
    FsModification* find_entry(const char* path);
    
    // Get the effective path (after renames)
    const char* resolve_path(const char* path, char* buffer);
    
    // Add a new modification
    void add_modification(const FsModification& mod);
    
    // Create a directory
    bool create_directory(const char* path, uint32_t mode);
    
    // Delete an entry
    bool delete_entry(const char* path);
    
    // Rename an entry
    bool rename_entry(const char* old_path, const char* new_path);
    
    // Get next fake inode number
    uint32_t get_fake_inode() { return next_fake_inode++; }
};

class Ext2 {
private: 
    StrongPtr<Ide> ide;
    uint32_t block_size;
    uint32_t inode_size;
    uint32_t inodes_per_group; 
    uint32_t blocks_per_group;
    uint32_t inodes_table_block;
    BlockCache* cache;  
public:
    StrongPtr<Node> root;
    
    Ext2(StrongPtr<Ide> ide);

    uint32_t get_block_size() {
        return block_size;
    }

    uint32_t get_inode_size() {
        return inode_size;
    }

    StrongPtr<Node> find(StrongPtr<Node> dir, const char* name);

private:
    StrongPtr<Node> find_internal(StrongPtr<Node> dir, const char* name);
};
