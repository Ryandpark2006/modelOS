# Remaining Syscalls for BusyBox Support

## Current Status: 30+ syscalls implemented ✅

## Additional Syscalls Needed (Priority Order)

### HIGH PRIORITY (Essential for BusyBox) - ~5 syscalls

1. **`lseek` (#19)** - Standard file positioning ⚠️ EASY
   - Time: 1 hour
   - Replace your custom `seek` (syscall #13)
   - BusyBox expects standard `lseek` with SEEK_SET/SEEK_CUR/SEEK_END
   
   ```cpp
   case 19: // lseek
   {
       int fd = (int)get_arg(frame, 0);
       int32_t offset = (int32_t)get_arg(frame, 1);
       int whence = (int)get_arg(frame, 2);
       
       if (fd < 0 || fd >= MAX_FDS || !fd_table[fd].in_use) {
           return -1;
       }
       
       uint32_t new_offset;
       if (whence == 0) {  // SEEK_SET
           new_offset = offset;
       } else if (whence == 1) {  // SEEK_CUR
           new_offset = fd_table[fd].offset + offset;
       } else if (whence == 2) {  // SEEK_END
           new_offset = fd_table[fd].node->size_in_bytes() + offset;
       } else {
           return -1;
       }
       
       fd_table[fd].offset = new_offset;
       return new_offset;
   }
   ```

2. **`dup` (#41)** - Duplicate file descriptor ⚠️ EASY
   - Time: 30 minutes
   - Needed for I/O redirection
   
   ```cpp
   case 41: // dup
   {
       int oldfd = (int)get_arg(frame, 0);
       
       if (oldfd < 0 || oldfd >= MAX_FDS || !fd_table[oldfd].in_use) {
           return -1;
       }
       
       // Find free fd
       int newfd = -1;
       for (int i = 0; i < MAX_FDS; i++) {
           if (!fd_table[i].in_use) {
               newfd = i;
               break;
           }
       }
       
       if (newfd == -1) {
           return -1;
       }
       
       // Copy fd entry
       fd_table[newfd] = fd_table[oldfd];
       fd_table[newfd].refcount++;
       
       return newfd;
   }
   ```

3. **`dup2` (#63)** - Duplicate to specific fd ⚠️ EASY
   - Time: 1 hour
   - Used for redirection (e.g., `ls > file`)
   
   ```cpp
   case 63: // dup2
   {
       int oldfd = (int)get_arg(frame, 0);
       int newfd = (int)get_arg(frame, 1);
       
       if (oldfd < 0 || oldfd >= MAX_FDS || !fd_table[oldfd].in_use) {
           return -1;
       }
       
       if (newfd < 0 || newfd >= MAX_FDS) {
           return -1;
       }
       
       if (oldfd == newfd) {
           return newfd;  // Already same
       }
       
       // Close newfd if it's open
       if (fd_table[newfd].in_use) {
           fd_table[newfd].refcount--;
           if (fd_table[newfd].refcount <= 0) {
               fd_table[newfd].in_use = false;
           }
       }
       
       // Copy oldfd to newfd
       fd_table[newfd] = fd_table[oldfd];
       fd_table[newfd].refcount++;
       
       return newfd;
   }
   ```

4. **`waitpid` (#7)** - Enhanced wait ⚠️ EASY
   - Time: 2 hours
   - More flexible than current `wait`
   - Supports WNOHANG option
   
   ```cpp
   case 7: // waitpid (different from shutdown which is also #7 in your system)
   {
       // Similar to your existing wait (syscall #8)
       // Add options parameter for WNOHANG support
       int pid = (int)get_arg(frame, 0);
       int* status = (int*)get_arg(frame, 1);
       int options = (int)get_arg(frame, 2);
       
       // If pid == -1, wait for any child (like wait)
       // If pid > 0, wait for specific child
       // If options & WNOHANG, return immediately if no child ready
       
       // Implementation similar to your current wait (syscall #8)
       // Just add options handling
   }
   ```

5. **`access` (#33)** - Check file accessibility ⚠️ EASY
   - Time: 1 hour
   - Can be stub initially (always return 0)
   
   ```cpp
   case 33: // access
   {
       const char* pathname = (const char*)get_arg(frame, 0);
       int mode = (int)get_arg(frame, 1);
       
       if (pathname == nullptr) {
           return -1;
       }
       
       // Simple implementation: just check if file exists
       if (global_fs == nullptr) {
           auto d = StrongPtr<Ide>::make(1,0);
           global_fs = StrongPtr<Ext2>::make(d);
       }
       
       auto node = global_fs->find(global_fs->root, pathname);
       if (node == nullptr) {
           return -1;  // File doesn't exist
       }
       
       // TODO: Check actual permissions based on mode
       // For now, just return success if file exists
       return 0;
   }
   ```

### MEDIUM PRIORITY (Useful for BusyBox) - ~5 syscalls

6. **`pipe` (#42)** - Create pipe 🔧 MODERATE
   - Time: 4-6 hours
   - Needed for command piping (`ls | grep`)
   - Requires pipe buffer implementation

7. **`getuid` / `getgid` (#24, #47)** - Get user/group ⚠️ TRIVIAL
   - Time: 5 minutes each
   - Just return 0 (root)
   
   ```cpp
   case 24: // getuid
       return 0;  // Always root
   
   case 47: // getgid
       return 0;  // Always root
   ```

8. **`geteuid` / `getegid` (#49, #50)** - Get effective user/group ⚠️ TRIVIAL
   - Time: 5 minutes each
   - Just return 0 (root)
   
   ```cpp
   case 49: // geteuid
       return 0;
   
   case 50: // getegid
       return 0;
   ```

9. **`readv` / `writev` (#145, #146)** - Scatter/gather I/O ⚠️ EASY
   - Time: 2-3 hours
   - Multiple read/write operations
   
   ```cpp
   case 145: // readv
   {
       int fd = (int)get_arg(frame, 0);
       struct iovec* iov = (struct iovec*)get_arg(frame, 1);
       int iovcnt = (int)get_arg(frame, 2);
       
       int total = 0;
       for (int i = 0; i < iovcnt; i++) {
           int n = read(fd, iov[i].iov_base, iov[i].iov_len);
           if (n < 0) return -1;
           total += n;
           if (n < (int)iov[i].iov_len) break;  // EOF or error
       }
       return total;
   }
   ```

10. **`fcntl` (#55)** - File control 🔧 COMPLEX
    - Time: 4-6 hours
    - Implement basic operations only (F_GETFD, F_SETFD, F_DUPFD)
    - BusyBox uses for fd flags

### LOW PRIORITY (Nice to have) - ~10 syscalls

11. **`uname` (#122)** - System information ⚠️ TRIVIAL
    - Time: 15 minutes
    - Return fixed strings
    
    ```cpp
    case 122: // uname
    {
        struct utsname {
            char sysname[65];
            char nodename[65];
            char release[65];
            char version[65];
            char machine[65];
        };
        
        struct utsname* buf = (struct utsname*)get_arg(frame, 0);
        if (buf == nullptr) return -1;
        
        strcpy(buf->sysname, "Prog8OS");
        strcpy(buf->nodename, "localhost");
        strcpy(buf->release, "1.0");
        strcpy(buf->version, "CS439 P8");
        strcpy(buf->machine, "i686");
        
        return 0;
    }
    ```

12-20. Additional syscalls as needed based on BusyBox utilities you want to run

---

## Summary

### Current Implementation
- ✅ 30+ syscalls working
- ⚠️ 5 syscalls are stubs (need Ext2 write support)

### To Add for BusyBox

| Priority | Syscalls | Time | Difficulty |
|----------|----------|------|------------|
| **HIGH** | 5 (lseek, dup, dup2, waitpid, access) | 5-6 hours | ⚠️ Easy |
| **MEDIUM** | 5 (pipe, uid/gid, readv/writev, fcntl) | 8-12 hours | 🔧 Moderate |
| **LOW** | 10+ (uname, etc.) | 10-15 hours | ⚠️ Easy-Moderate |

### Total Additions Needed
- **Minimum (HIGH only):** 5 syscalls, ~6 hours
- **Good coverage (HIGH + MEDIUM):** 10 syscalls, ~18 hours
- **Comprehensive (all):** 20+ syscalls, ~30 hours

---

## Recommended Implementation Order

1. **This week:**
   - `lseek` (1 hour)
   - `dup`, `dup2` (2 hours)
   - `getuid`, `getgid`, `geteuid`, `getegid` (30 min)
   - `access` (1 hour)
   - **Total: ~5 hours for 7 syscalls**

2. **Next week:**
   - `waitpid` (2 hours)
   - `readv`, `writev` (3 hours)
   - `uname` (30 min)
   - **Total: ~6 hours for 4 syscalls**

3. **Week after:**
   - `pipe` (6 hours)
   - `fcntl` (basic) (4 hours)
   - **Total: ~10 hours for 2 syscalls**

4. **Then:**
   - Implement Ext2 write support (1-2 weeks)
   - Complete mkdir/rmdir/unlink/rename
   - Test with full BusyBox

---

## Quick Wins (Do This First!)

Add these 7 syscalls this week (5-6 hours total):

```cpp
// In sys.cc, add before default case:

case 19: // lseek
    // See implementation above

case 41: // dup
    // See implementation above

case 63: // dup2
    // See implementation above

case 24: // getuid
    return 0;

case 47: // getgid
    return 0;

case 49: // geteuid
    return 0;

case 50: // getegid
    return 0;

case 33: // access
    // See implementation above
```

This brings you to **37+ syscalls** - enough for basic BusyBox!







