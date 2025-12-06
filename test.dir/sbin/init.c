#include "libc.h"

// Test helper: try to exec a file in a child process
void test_exec_in_child(const char* path, const char* test_name) {
    printf("| Test: %s\n", test_name);

    int id = fork();
    if (id < 0) {
        printf("| fork failed for test: %s\n", test_name);
        return;
    } else if (id == 0) {
        /* child process */
        printf("| Child attempting to exec: %s\n", path);
        int rc = execl(path, path, 0);
        printf("| execl failed with rc=%d (expected for invalid ELF)\n", rc);
        exit(1);
    } else {
        /* parent process */
        uint32_t status = 0;
        printf("| Parent waiting for child (pid=%d)\n", id);
        int wait_rc = wait(id, &status);
        printf("| Child exited with status=%lu, wait_rc=%d\n", (unsigned long)status, wait_rc);
    }
}

// Test helper: read and display ELF header info
void inspect_elf_header(const char* path) {
    printf("| Inspecting ELF header of: %s\n", path);
    
    int fd = open(path, 0);
    if (fd < 0) {
        printf("| Failed to open file\n");
        return;
    }
    
    printf("| File descriptor: %d\n", fd);
    printf("| File length: %d bytes\n", len(fd));
    
    // Read first 52 bytes (ELF header for 32-bit)
    char header[64];
    memset(header, 0, 64);
    ssize_t bytes_read = read(fd, header, 52);
    printf("| Read %d bytes from ELF header\n", bytes_read);
    
    if (bytes_read >= 4) {
        printf("| Magic bytes: 0x%02x 0x%02x 0x%02x 0x%02x\n", 
               (unsigned char)header[0], 
               (unsigned char)header[1], 
               (unsigned char)header[2], 
               (unsigned char)header[3]);
    }
    
    close(fd);
}

int main(int argc, char** argv) {
    /* Only a single starred start message */
    printf("*** starting elf validation\n");
    /* Other informational output uses '|' prefix for debug */
    printf("| argc = %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("| argv[%d] = %s\n", i, argv[i]);
    }

    // Test 1: Attempt to exec a valid ELF (should succeed)
    printf("| === Test 1: Valid ELF execution ===\n");
    inspect_elf_header("/sbin/shell");
    test_exec_in_child("/sbin/shell", "Valid ELF - /sbin/shell");

    // Test 2: Attempt to exec non-existent file
    printf("| === Test 2: Non-existent file ===\n");
    test_exec_in_child("/sbin/nonexistent", "Non-existent file");

    // Test 3: Attempt to exec a non-ELF file (text file)
    printf("| === Test 3: Non-ELF file (text) ===\n");
    inspect_elf_header("/etc/data.txt");
    test_exec_in_child("/etc/data.txt", "Non-ELF text file");

    // Test 4: Attempt to exec another text file
    printf("| === Test 4: Another non-ELF file ===\n");
    if (open("/hello", 0) >= 0) {
        inspect_elf_header("/hello");
        test_exec_in_child("/hello", "Text file /hello");
        close(3);
    }

    // Test 5: Multiple rapid fork-exec attempts
    printf("| === Test 5: Multiple invalid exec attempts ===\n");
    for (int i = 0; i < 3; i++) {
        printf("| Iteration %d\n", i);
        test_exec_in_child("/goodbye", "Rapid test iteration");
    }

    // Test 6: Attempt to exec after opening and inspecting
    printf("| === Test 6: Exec after file inspection ===\n");
    int fd = open("/sbin/shell", 0);
    if (fd >= 0) {
        printf("| Opened /sbin/shell with fd=%d\n", fd);
        char buf[100];
        read(fd, buf, 50);
        printf("| Read some bytes, now closing and attempting exec\n");
        close(fd);
    }
    test_exec_in_child("/sbin/shell", "Exec after file inspection");

    // Test 7: ELF with wrong class (64-bit instead of 32-bit)
    printf("| === Test 7: Wrong ELF class (64-bit) ===\n");
    inspect_elf_header("/data/wrong_class.elf");
    test_exec_in_child("/data/wrong_class.elf", "64-bit ELF (class=2)");

    // Test 8: ELF with wrong endianness (big-endian instead of little-endian)
    printf("| === Test 8: Wrong endianness ===\n");
    inspect_elf_header("/data/wrong_endian.elf");
    test_exec_in_child("/data/wrong_endian.elf", "Big-endian ELF");

    // Test 9: ELF that's relocatable, not executable
    printf("| === Test 9: Relocatable ELF (not executable) ===\n");
    inspect_elf_header("/data/not_executable.elf");
    test_exec_in_child("/data/not_executable.elf", "Relocatable ELF (type=1)");

    // Test 10: ELF for wrong architecture (ARM instead of i386)
    printf("| === Test 10: Wrong architecture ===\n");
    inspect_elf_header("/data/wrong_arch.elf");
    test_exec_in_child("/data/wrong_arch.elf", "ARM ELF (machine=40)");

    // Test 11: Truncated ELF file (only 20 bytes)
    printf("| === Test 11: Truncated ELF ===\n");
    inspect_elf_header("/data/truncated.elf");
    test_exec_in_child("/data/truncated.elf", "Truncated ELF (20 bytes)");

    // Test 12: File with all zeros (no magic)
    printf("| === Test 12: All zeros file ===\n");
    inspect_elf_header("/data/all_zeros.elf");
    test_exec_in_child("/data/all_zeros.elf", "All zeros (no magic)");

    // Test 13: Plain text file clearly not ELF
    printf("| === Test 13: Plain text ===\n");
    inspect_elf_header("/data/not_elf.txt");
    test_exec_in_child("/data/not_elf.txt", "Plain text file");

    // Test 14: Try to exec a directory
    printf("| === Test 14: Directory (not a file) ===\n");
    test_exec_in_child("/data", "Directory instead of file");

    // Test 15: Empty path
    printf("| === Test 15: Empty/null path ===\n");
    test_exec_in_child("", "Empty path string");

    printf("*** done\n");
    shutdown();
    return 0;
}
