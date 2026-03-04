#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            return EXIT_FAILURE;
        }
        if (pid == 0) {
            srand(time(NULL) + i);
            printf("child %d starts: ", i);
            for (int j = 0; j < 3; j++) {
                printf("%d, ", 1 + (rand() % 6));
            }
            printf("\n");
            return EXIT_SUCCESS;
        }
    }
}
