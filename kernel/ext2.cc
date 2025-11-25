#include "ext2.h"
#include "libk.h"

// Superblock structure (starts at byte 1024, size 1024 bytes) 
struct ext2_superblock {
    uint32_t s_inodes_count;        
    uint32_t s_blocks_count;        
    uint32_t s_r_blocks_count;      
    uint32_t s_free_blocks_count;   
    uint32_t s_free_inodes_count;   
    uint32_t s_first_data_block;    
    uint32_t s_log_block_size;      
    uint32_t s_log_frag_size;       
    uint32_t s_blocks_per_group;    
    uint32_t s_frags_per_group;     
    uint32_t s_inodes_per_group;    
    uint32_t s_mtime;               
    uint32_t s_wtime;               
    uint16_t s_mnt_count;           
    uint16_t s_max_mnt_count;       
    uint16_t s_magic;               
    uint16_t s_state;               
    uint16_t s_errors;              
    uint16_t s_minor_rev_level;     
    uint32_t s_lastcheck;           
    uint32_t s_checkinterval;       
    uint32_t s_creator_os;          
    uint32_t s_rev_level;           
    uint16_t s_def_resuid;          
    uint16_t s_def_resgid;          
    uint32_t s_first_ino;           
    uint16_t s_inode_size;          
    uint16_t s_block_group_nr;      
    uint32_t s_feature_compat;      
    uint32_t s_feature_incompat;    
    uint32_t s_feature_ro_compat;   
    uint8_t  s_uuid[16];            
    char     s_volume_name[16];     
    char     s_last_mounted[64];    
    uint32_t s_algo_bitmap;         
    uint8_t  s_padding[824];        
} __attribute__((packed));

// block group descriptor
struct ext2_group_desc {
    uint32_t bg_block_bitmap;
    uint32_t bg_inode_bitmap;
    uint32_t bg_inode_table;
    uint16_t bg_free_blocks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;
} __attribute__((packed));

Node::Node(StrongPtr<Ide> ide_ptr, uint32_t inode_num, uint32_t fs_blk_size,
           uint32_t inode_sz, uint32_t inode_tbl_blk, BlockCache* cache_ptr)
    : BlockIO(fs_blk_size),              
      ide(ide_ptr),                       
      fs_block_size(fs_blk_size),        
      inode_table_block(inode_tbl_blk),
      cache(cache_ptr),
      number(inode_num) {                
    
    uint32_t inode_index = inode_num - 1; // inode number is 1-indexed
    
    uint32_t inode_offset_in_table = inode_index * inode_sz; 
    
    uint32_t inode_block = inode_table_block + (inode_offset_in_table / fs_block_size);
    
    uint32_t offset_in_block = inode_offset_in_table % fs_block_size;
    
    char* block_buffer = new char[fs_block_size];
    
    uint32_t byte_offset = inode_block * fs_block_size;
    uint32_t sectors_per_block = fs_block_size / 512;
    uint32_t start_sector = byte_offset / 512;
    
    for (uint32_t i = 0; i < sectors_per_block; i++) {
        ide->read_block(start_sector + i, block_buffer + (i * 512));
    }
    
    memcpy(&inode, block_buffer + offset_in_block, sizeof(ext2_inode));
    
    delete[] block_buffer;
    
}

Ext2::Ext2(StrongPtr<Ide> ide) : ide(ide) {  
    // reading superblock
    ext2_superblock sb;

    // superblock is at byte 1024
    char sb_buffer[1024];
    ide->read_block(2, sb_buffer);
    ide->read_block(3, sb_buffer + 512);

    memcpy(&sb, sb_buffer, sizeof(ext2_superblock));

    if (sb.s_magic != EXT2_SUPER_MAGIC) {
        Debug::panic("Not a valid ext2 filesystem! Magic = 0x%x", sb.s_magic);
    }

    block_size = 1024 << sb.s_log_block_size;
    inode_size = sb.s_inode_size;
    if (inode_size == 0) {
        inode_size = 128;  
    }
    inodes_per_group = sb.s_inodes_per_group;
    blocks_per_group = sb.s_blocks_per_group;

    cache = new BlockCache(block_size, 32);  

    uint32_t bgd_block = (block_size == 1024) ? 2 : 1;
        
    // byte offset for bgd
    uint32_t bgd_byte_offset = bgd_block * block_size;
    
    // size of bgd struct 
    char bgd_buffer[512];
    
    // block to ide sector 
    uint32_t start_sector = bgd_byte_offset / 512;
    ide->read_block(start_sector, bgd_buffer);
    
    ext2_group_desc bgd;
    memcpy(&bgd, bgd_buffer, sizeof(ext2_group_desc));
    
    inodes_table_block = bgd.bg_inode_table;
    
    // root node at inode 2 
    root = StrongPtr<Node>::make(ide, EXT2_ROOT_INO, block_size, 
                                  inode_size, inodes_table_block, cache);
}

StrongPtr<Node> Ext2::find(StrongPtr<Node> dir, const char* name) {
    return find_internal(dir, name);
}

StrongPtr<Node> Ext2::find_internal(StrongPtr<Node> dir, const char* name) {
    if (dir == nullptr) {
        return {};
    }
    
    uint32_t path_len = K::strlen(name);
    char* path = new char[4096]; 
    for (uint32_t i = 0; i <= path_len; i++) {
        path[i] = name[i];
    }
    
    StrongPtr<Node> current = dir;
    uint32_t symlinks_followed = 0;
    
    while (true) {
        if (path[0] == '/') {
            current = root;
            while (path[0] == '/') {
                uint32_t i = 0;
                while (path[i + 1] != '\0') {
                    path[i] = path[i + 1];
                    i++;
                }
                path[i] = '\0';
            }
        }
        
        if (path[0] == '\0') {
            delete[] path;
            return current;
        }
        
        if (!current->is_dir()) {
            delete[] path;
            return {};
        }
        
        uint32_t component_len = 0;
        while (path[component_len] != '\0' && path[component_len] != '/') {
            component_len++;
        }
        
        char component[256];  
        for (uint32_t i = 0; i < component_len; i++) {
            component[i] = path[i];
        }
        component[component_len] = '\0';
        
        StrongPtr<Node> next_node = {};
        uint32_t dir_size = current->size_in_bytes();
        uint32_t bytes_read = 0;
        char* block_buffer = new char[block_size];
        uint32_t block_index = 0;
        
        while (bytes_read < dir_size && next_node == nullptr) {
            current->read_block(block_index, block_buffer);
            
            uint32_t offset = 0;
            while (offset < block_size && bytes_read < dir_size) {
                ext2_dir_entry* entry = (ext2_dir_entry*)(block_buffer + offset);
                
                if (entry->rec_len == 0) {
                    break;
                }
                
                if (entry->inode != 0 && entry->name_len == component_len) {
                    bool match = true;
                    for (uint32_t i = 0; i < component_len; i++) {
                        if (entry->name[i] != component[i]) {
                            match = false;
                            break;
                        }
                    }
                    
                    if (match) {
                        next_node = StrongPtr<Node>::make(ide, entry->inode, block_size,
                                                          inode_size, inodes_table_block, cache);
                        break;
                    }
                }
                
                offset += entry->rec_len;
                bytes_read += entry->rec_len;
            }
            
            block_index++;
        }
        
        delete[] block_buffer;
        
        if (next_node == nullptr) {
            delete[] path;
            return {};
        }
        
        uint32_t j = component_len;
        while (path[j] == '/') {
            j++;  
        }
        bool has_more_path = (path[j] != '\0');
        
        uint32_t i = 0;
        while (path[j] != '\0') {
            path[i++] = path[j++];
        }
        path[i] = '\0';
    
        if (next_node->is_symlink() && has_more_path) {
            symlinks_followed++;
            if (symlinks_followed > 100) { // prevents infinite recursion
                delete[] path;
                return {};
            }
            
            uint32_t link_size = next_node->size_in_bytes();
            char link_target[4096];
            next_node->get_symbol(link_target);
            link_target[link_size] = '\0';
            
            char new_path[4096];
            uint32_t new_len = 0;
            
            for (uint32_t i = 0; i < link_size; i++) {
                new_path[new_len++] = link_target[i];
            }
            
            if (path[0] != '\0') {
                if (link_size > 0 && link_target[link_size - 1] != '/' && path[0] != '/') {
                    new_path[new_len++] = '/';
                }
                for (uint32_t i = 0; path[i] != '\0'; i++) {
                    new_path[new_len++] = path[i];
                }
            }
            new_path[new_len] = '\0';
            
            for (uint32_t i = 0; i <= new_len; i++) {
                path[i] = new_path[i];
            }
            
            if (link_target[0] == '/') {
                current = root;
            }
            continue;
        }
        
        current = next_node;
    }
}

void Node::get_symbol(char* buffer) {
    if (!is_symlink()) {
        Debug::panic("get_symbol called on non-symlink!");
    }
    
    uint32_t size = inode.i_size;
    
    if (size < 60) {
        char* link_target = (char*)inode.i_block;
        memcpy(buffer, link_target, size);
        buffer[size] = '\0';  
    }
    else {
        int64_t bytes_read = read_all(0, size, buffer);
        if (bytes_read != (int64_t)size) {
            Debug::panic("Failed to read symlink target!");
        }
        buffer[size] = '\0';  
    }
}

void Node::read_block(uint32_t block_index, char* buffer) {
    uint32_t physical_block = 0;
    uint32_t ptrs_per_block = fs_block_size / sizeof(uint32_t);  
    
    if (block_index < 12) {
        physical_block = inode.i_block[block_index];
    }
    else if (block_index < 12 + ptrs_per_block) {
        uint32_t indirect_block = inode.i_block[12];
        
        if (indirect_block == 0) {
            physical_block = 0;
        } else {
            uint32_t indirect_index = block_index - 12;
            
            char* indirect_buffer = new char[fs_block_size];
            read_fs_block(indirect_block, indirect_buffer);
            
            uint32_t* pointers = (uint32_t*)indirect_buffer;
            physical_block = pointers[indirect_index];
            
            delete[] indirect_buffer;
        }
    }
    else if (block_index < 12 + ptrs_per_block + (ptrs_per_block * ptrs_per_block)) {
        uint32_t double_indirect_block = inode.i_block[13];
        
        if (double_indirect_block == 0) {
            physical_block = 0;
        } else {
            uint32_t double_index = block_index - 12 - ptrs_per_block;
            uint32_t first_level_index = double_index / ptrs_per_block;
            uint32_t second_level_index = double_index % ptrs_per_block;
            
            char* first_indirect_buffer = new char[fs_block_size];
            read_fs_block(double_indirect_block, first_indirect_buffer);
            uint32_t* first_pointers = (uint32_t*)first_indirect_buffer;
            uint32_t second_indirect_block = first_pointers[first_level_index];
            delete[] first_indirect_buffer;
            
            if (second_indirect_block == 0) {
                physical_block = 0;
            } else {
                char* second_indirect_buffer = new char[fs_block_size];
                read_fs_block(second_indirect_block, second_indirect_buffer);
                uint32_t* second_pointers = (uint32_t*)second_indirect_buffer;
                physical_block = second_pointers[second_level_index];
                delete[] second_indirect_buffer;
            }
        }
    }
    else if (block_index < 12 + ptrs_per_block + (ptrs_per_block * ptrs_per_block) + (ptrs_per_block * ptrs_per_block * ptrs_per_block)) {
        uint32_t triple_indirect_block = inode.i_block[14];
        
        if (triple_indirect_block == 0) {
            physical_block = 0;
        } else {
            uint32_t triple_index = block_index - 12 - ptrs_per_block - (ptrs_per_block * ptrs_per_block);
            uint32_t first_level_index = triple_index / (ptrs_per_block * ptrs_per_block);
            uint32_t remainder = triple_index % (ptrs_per_block * ptrs_per_block);
            uint32_t second_level_index = remainder / ptrs_per_block;
            uint32_t third_level_index = remainder % ptrs_per_block;
            
            char* first_indirect_buffer = new char[fs_block_size];
            read_fs_block(triple_indirect_block, first_indirect_buffer);
            uint32_t* first_pointers = (uint32_t*)first_indirect_buffer;
            uint32_t second_indirect_block = first_pointers[first_level_index];
            delete[] first_indirect_buffer;
            
            if (second_indirect_block == 0) {
                physical_block = 0;
            } else {
                char* second_indirect_buffer = new char[fs_block_size];
                read_fs_block(second_indirect_block, second_indirect_buffer);
                uint32_t* second_pointers = (uint32_t*)second_indirect_buffer;
                uint32_t third_indirect_block = second_pointers[second_level_index];
                delete[] second_indirect_buffer;
                
                if (third_indirect_block == 0) {
                    physical_block = 0;
                } else {
                    char* third_indirect_buffer = new char[fs_block_size];
                    read_fs_block(third_indirect_block, third_indirect_buffer);
                    uint32_t* third_pointers = (uint32_t*)third_indirect_buffer;
                    physical_block = third_pointers[third_level_index];
                    delete[] third_indirect_buffer;
                }
            }
        }
    }
    else {
        Debug::panic("Block index %d exceeds maximum file size!", block_index);
    }
    
    if (physical_block == 0) {
        for (uint32_t i = 0; i < fs_block_size; i++) {
            buffer[i] = 0;
        }
    } else {
        read_fs_block(physical_block, buffer);
    }
}

void Node::read_fs_block(uint32_t fs_block_num, char* buffer) {
    if (cache && cache->read(fs_block_num, buffer)) {
        return;  
    }
    
    uint32_t byte_offset = fs_block_num * fs_block_size;
    uint32_t sectors_per_block = fs_block_size / 512;
    uint32_t start_sector = byte_offset / 512;
    
    for (uint32_t i = 0; i < sectors_per_block; i++) {
        ide->read_block(start_sector + i, buffer + (i * 512));
    }
    
    if (cache) {
        cache->write(fs_block_num, buffer);
    }
}

uint32_t Node::entry_count() {
    if (!is_dir()) {
        Debug::panic("entry_count called on non-directory!");
    }
    
    uint32_t count = 0;
    uint32_t bytes_read = 0;
    uint32_t dir_size = inode.i_size;
    
    char* block_buffer = new char[fs_block_size];
    
    uint32_t block_index = 0;
    while (bytes_read < dir_size) {
        read_block(block_index, block_buffer);
        
        uint32_t offset = 0;
        while (offset < fs_block_size && bytes_read < dir_size) {
            ext2_dir_entry* entry = (ext2_dir_entry*)(block_buffer + offset);
            
            if (entry->rec_len == 0) {
                break;  
            }
            
            if (entry->inode != 0) {
                count++;
            }
            
            offset += entry->rec_len;
            bytes_read += entry->rec_len;
        }
        
        block_index++;
    }
    
    delete[] block_buffer;
    return count;
}

