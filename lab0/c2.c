#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        return EXIT_FAILURE;
    }
    char *filename = argv[1];
    int howmany = atoi(argv[2]);

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        return EXIT_FAILURE;
    }
    int len = 0;
    fscanf(f, "%d\n", &len);

    double *vec = malloc(sizeof(double) * len);
    if (vec == NULL) {
        fclose(f);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < len; i++) {
        fscanf(f, "%lf;", &vec[i]);
    }
    fclose(f);

    int lenleft = len;
    int offset = 0;

    for (int i = 0; i < howmany; i++) {
        int size = ceil(lenleft / (double)howmany);
        if (i == howmany - 1) {
            size = lenleft;
        }
        pid_t pid = fork();
        if (pid == -1) {
            free(vec);
            return EXIT_FAILURE;
        }
        if (pid == 0) {
            double max = vec[offset];
            for (int ij = 1; ij < size; ij++) {
                if (vec[offset + ij] > max) {
                    max = vec[offset + ij];
                }
            }
            printf("max of ");
            for (int ij = 0; ij < size; ij++) {
                printf("%lf, ", vec[offset + ij]);
            }
            printf("\n is %lf\n", max);
            return EXIT_SUCCESS;
        }
        lenleft -= size;
        offset += size;
    }

    for (int i = 0; i < howmany; i++) {
        if (wait(0) == -1) {
            free(vec);
            return EXIT_FAILURE;
        }
    }

    free(vec);
}
