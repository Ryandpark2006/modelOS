# Filesystem Write Implementation Plan

## Goal
Implement write operations (mkdir, rmdir, unlink, rename) using an in-memory overlay approach that works per-process without persisting to disk.

## Strategy: Copy-on-Write Filesystem Overlay

Instead of implementing full Ext2 write support (which is complex), we'll create an in-memory overlay:

### Architecture
```
User Process
     ↓
Syscall (mkdir/rmdir/unlink/rename)
     ↓
FilesystemOverlay (per-process modifications)
     ↓
Ext2 (read-only base filesystem)
```

### Data Structures

```cpp
// Per-process filesystem overlay
struct FsModification {
    enum Type { CREATE_DIR, CREATE_FILE, DELETE, RENAME } type;
    char path[256];
    char new_path[256];  // For rename
    uint32_t fake_inode;  // For created entries
    mode_t mode;
};

class FilesystemOverlay {
    // Store modifications per process
    Vector<FsModification> modifications;
    uint32_t next_fake_inode = 1000000;  // Start high to avoid conflicts
    
    // Check if path is deleted
    bool is_deleted(const char* path);
    
    // Check if path exists in overlay
    FsModification* find_modification(const char* path);
    
    // Add a modification
    void add_modification(FsModification mod);
};
```

## Implementation Steps

### 1. Create Filesystem Overlay Class
- Add to `ext2.h` and `ext2.cc`
- Store in `UserProcessTCB` (per-process)

### 2. Implement mkdir
```cpp
case 39: // mkdir
    1. Parse path
    2. Check if parent exists
    3. Check if target already exists
    4. Add CREATE_DIR modification to overlay
    5. Return 0 on success
```

### 3. Implement rmdir
```cpp
case 40: // rmdir
    1. Parse path
    2. Check if directory exists
    3. Check if directory is empty
    4. Add DELETE modification to overlay
    5. Return 0 on success
```

### 4. Implement unlink
```cpp
case 87: // unlink
    1. Parse path
    2. Check if file exists
    3. Check it's not a directory
    4. Add DELETE modification to overlay
    5. Return 0 on success
```

### 5. Implement rename
```cpp
case 38: // rename
    1. Parse old and new paths
    2. Check if old path exists
    3. Check if new path parent exists
    4. Add RENAME modification to overlay
    5. Return 0 on success
```

### 6. Update stat/open/etc to Check Overlay
Modify existing syscalls to check overlay first:
- If path is deleted in overlay, return -ENOENT
- If path is created in overlay, return fake inode info
- Otherwise, fall through to Ext2

## Benefits of This Approach

✅ **Simple** - No Ext2 write complexity  
✅ **Fast** - All in-memory  
✅ **Safe** - Can't corrupt disk  
✅ **Isolated** - Per-process modifications  
✅ **Testable** - Easy to validate  

## Limitations

⚠️ **Not persistent** - Changes lost on process exit  
⚠️ **Not shared** - Other processes don't see changes  
⚠️ **Limited** - Can't create files with content  
⚠️ **Simplified** - No hard links, permissions, etc.  

But this is PERFECT for testing and meets the "private address space" requirement!

## Files to Modify

1. `kernel/ext2.h` - Add FilesystemOverlay class
2. `kernel/ext2.cc` - Implement FilesystemOverlay methods
3. `kernel/process.h` - Add overlay to UserProcessTCB
4. `kernel/sys.cc` - Implement mkdir/rmdir/unlink/rename syscalls
5. `kernel/sys.cc` - Update stat/open to check overlay

## Testing Strategy

After implementation, the functional tests should show:
- mkdir: Creates directory (returns 0)
- rmdir: Removes directory (returns 0)
- unlink: Deletes file (returns 0)
- rename: Renames file (returns 0)

We can verify with:
- stat() on created paths returns valid info
- stat() on deleted paths returns -1
- Operations respect parent directory existence

Let's implement this!

