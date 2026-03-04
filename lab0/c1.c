#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
        for (int i = 0; i < 5; i++) {
                pid_t pid = fork();
                printf("Pid: %d\n", pid);
                if (pid == -1) {
                        return EXIT_FAILURE;
                }
                if (pid != 0) {
                        printf("%d sleeps\n", pid);
                        if(wait(0) == -1) {
                                return EXIT_FAILURE;
                        };
                        break;
                }
        }
        return EXIT_SUCCESS;
}
