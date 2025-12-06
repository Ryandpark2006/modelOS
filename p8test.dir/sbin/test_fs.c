#include "sys.h"

// Custom print functions
static int my_strlen(const char* s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

void print(const char* msg) {
    int len = my_strlen(msg);
    write(1, (void*)msg, len);
}

void print_num(int n) {
    if (n < 0) {
        print("-");
        n = -n;
    }
    if (n >= 10) {
        print_num(n / 10);
    }
    char c = '0' + (n % 10);
    write(1, &c, 1);
}

int main() {
    print("*** Filesystem Write Operations Test ***\n");
    
    // Test 1: mkdir
    print("\n1. Testing mkdir:\n");
    int ret = mkdir("/testdir", 0755);
    print("   mkdir(\"/testdir\", 0755) = ");
    print_num(ret);
    print((ret == 0) ? " PASS\n" : " FAIL\n");
    
    // Test 2: stat on created directory
    print("\n2. Testing stat on created dir:\n");
    struct stat st;
    ret = stat("/testdir", &st);
    print("   stat(\"/testdir\") = ");
    print_num(ret);
    if (ret == 0) {
        print(" PASS (inode=");
        print_num(st.st_ino);
        print(", mode=");
        print_num(st.st_mode);
        print(")\n");
    } else {
        print(" FAIL\n");
    }
    
    // Test 3: mkdir duplicate (should fail)
    print("\n3. Testing mkdir duplicate:\n");
    ret = mkdir("/testdir", 0755);
    print("   mkdir(\"/testdir\", 0755) again = ");
    print_num(ret);
    print((ret == -1) ? " PASS (correctly failed)\n" : " FAIL (should have failed)\n");
    
    // Test 4: rename
    print("\n4. Testing rename:\n");
    ret = rename("/testdir", "/newdir");
    print("   rename(\"/testdir\", \"/newdir\") = ");
    print_num(ret);
    print((ret == 0) ? " PASS\n" : " FAIL\n");
    
    // Test 5: stat on renamed directory
    print("\n5. Testing stat on renamed dir:\n");
    ret = stat("/newdir", &st);
    print("   stat(\"/newdir\") = ");
    print_num(ret);
    print((ret == 0) ? " PASS\n" : " FAIL\n");
    
    // Test 6: stat on old name (should fail)
    print("\n6. Testing stat on old name:\n");
    ret = stat("/testdir", &st);
    print("   stat(\"/testdir\") = ");
    print_num(ret);
    print((ret == -1) ? " PASS (correctly failed)\n" : " FAIL (should have failed)\n");
    
    // Test 7: rmdir
    print("\n7. Testing rmdir:\n");
    ret = rmdir("/newdir");
    print("   rmdir(\"/newdir\") = ");
    print_num(ret);
    print((ret == 0) ? " PASS\n" : " FAIL\n");
    
    // Test 8: stat on deleted directory (should fail)
    print("\n8. Testing stat on deleted dir:\n");
    ret = stat("/newdir", &st);
    print("   stat(\"/newdir\") = ");
    print_num(ret);
    print((ret == -1) ? " PASS (correctly failed)\n" : " FAIL (should have failed)\n");
    
    // Test 9: Create another directory
    print("\n9. Testing mkdir another dir:\n");
    ret = mkdir("/testdir2", 0755);
    print("   mkdir(\"/testdir2\", 0755) = ");
    print_num(ret);
    print((ret == 0) ? " PASS\n" : " FAIL\n");
    
    // Test 10: unlink on directory (should fail)
    print("\n10. Testing unlink on dir (should fail):\n");
    ret = unlink("/testdir2");
    print("   unlink(\"/testdir2\") = ");
    print_num(ret);
    print((ret == -1) ? " PASS (correctly failed)\n" : " FAIL (should have failed)\n");
    
    print("\n*** Filesystem Test Complete! ***\n");
    shutdown();
    return 0;
}

