#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////

int main() {
    const char *directory = "randomcanary.elevate";

    printf("--- Running 'id' command before rmdir ---\n");
    if (system("id") == -1) {
        perror("Failed to execute 'id'");
        return 1;
    }

    syscall(SYS_rmdir, directory);

    printf("\n--- Running 'id' command after rmdir ---\n");
    if (system("id") == -1) {
        perror("Failed to execute 'id'");
        return 1;
    }

    return 0;
}
