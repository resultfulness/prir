#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/param.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define RANGE_KEY 1001
#define RESULT_KEY 1002
#define VEC_KEY 1003

int child_id;

void on_sigusr1(int signo)
{
        int range_shm_id = shmget(RANGE_KEY, 0, 0666);
        if (range_shm_id == -1) {
                fprintf(stderr, "shmget failed\n");
                goto sigout;
        }

        int result_shm_id = shmget(RESULT_KEY, 0, 0666);
        if (result_shm_id == -1) {
                fprintf(stderr, "shmget failed\n");
                goto sigout;
        }

        int vec_shm_id = shmget(VEC_KEY, 0, 0666);
        if (vec_shm_id == -1) {
                fprintf(stderr, "shmget failed\n");
                goto sigout;
        }

        int* range_shm;
        if ((range_shm = shmat(range_shm_id, NULL, 0)) == (int*)-1) {
                fprintf(stderr, "shmat failed\n");
                goto sigout;
        };
        double* result_shm;
        if ((result_shm = shmat(result_shm_id, NULL, 0)) == (double*)-1) {
                fprintf(stderr, "shmat failed\n");
                goto sigout_dt_range;
        };
        double* vec_shm;
        if ((vec_shm = shmat(vec_shm_id, NULL, 0)) == (double*)-1) {
                fprintf(stderr, "shmat failed\n");
                goto sigout_dt_result;
        };

        int start = range_shm[child_id * 2];
        int end = range_shm[child_id * 2 + 1];

        //printf("[%d] my range is %d -- %d\n", child_id, start, end);
        double total = 0.0;
        for (int i = start; i <= end; i++) {
                //printf("[%d] adding %lf\n", child_id, vec_shm[i]);
                total += vec_shm[i];
        }
        result_shm[child_id] = total;

        // sigout_dt_vec:
        shmdt(vec_shm);
sigout_dt_result:
        shmdt(result_shm);
sigout_dt_range:
        shmdt(range_shm);

sigout:
        return;
}

int main(int argc, char** argv)
{
        if (argc < 3) {
                fprintf(stderr, "Usage: %s <n processes> <filename>\n", argv[0]);
                return EXIT_FAILURE;
        }

        int ret = EXIT_FAILURE;
        int N = atoi(argv[1]);
        if (N <= 0) {
                fprintf(stderr, "<n processes> must be at least 1\n");
                return EXIT_FAILURE;
        }

        char* filename = argv[2];

        pid_t* children = malloc(sizeof(pid_t) * N);
        if (children == NULL) {
                fprintf(stderr, "malloc failed\n");
                return EXIT_FAILURE;
        }

        int pd[2];
        if(pipe(pd) == -1) {
                fprintf(stderr, "Failed to open unnamed pipe\n");
                goto out_free_children;
        }


        pid_t pid;
        struct sigaction sa;
        sigset_t mask;
        for (int i = 0; i < N; i++) {
                switch (pid = fork()) {
                case -1:
                        fprintf(stderr, "fork failure\n");
                        for (int j = 0; j < i; j++) {
                                kill(children[j], SIGTERM);
                        }
                        for (int j = 0; j < i; j++) {
                                wait(0);
                        }
                        close(pd[0]);
                        close(pd[1]);
                        free(children);
                        return EXIT_FAILURE;
                case 0:

                        sa.sa_handler = &on_sigusr1;
                        sigemptyset(&mask);
                        sa.sa_mask = mask;
                        sa.sa_flags = SA_RESTART;

                        if (sigaction(SIGUSR1, &sa, NULL) == -1) {
                                fprintf(stderr, "sigaction failed\n");
                                return EXIT_FAILURE;
                        }

                        child_id = i;

                        write(pd[1], "k", 1);

                        close(pd[0]);
                        close(pd[1]);

                        pause();

                        return EXIT_SUCCESS;
                default:
                        children[i] = pid;
                }
        }

        FILE* f = fopen(filename, "r");
        if (f == NULL) {
                fprintf(stderr, "couldn't open file %s\n", filename);
                goto out_free_children;
        }

        int veclen = 0;
        fscanf(f, "%d\n", &veclen);
        if (veclen == 0) {
                fprintf(stderr, "file invalid\n");
                goto out_close_file;
        }

        int range_shm_id = shmget(RANGE_KEY, N * 2 * sizeof(int), IPC_CREAT | 0666);
        if (range_shm_id == -1) {
                fprintf(stderr, "shmget failed\n");
                goto out_close_file;
        }

        int result_shm_id = shmget(RESULT_KEY, N * sizeof(double), IPC_CREAT | 0666);
        if (result_shm_id == -1) {
                fprintf(stderr, "shmget failed\n");
                goto out_rm_range_shm;
        }

        int vec_shm_id = shmget(VEC_KEY, veclen * sizeof(double), IPC_CREAT | 0666);
        if (vec_shm_id == -1) {
                fprintf(stderr, "shmget failed\n");
                goto out_rm_result_shm;
        }

        int* range_shm;
        if ((range_shm = shmat(range_shm_id, NULL, 0)) == (int*)-1) {
                fprintf(stderr, "shmat failed\n");
                goto out_rm_vec_shm;
        };
        double* result_shm;
        if ((result_shm = shmat(result_shm_id, NULL, 0)) == (double*)-1) {
                fprintf(stderr, "shmat failed\n");
                goto out_dt_range;
        };
        double* vec_shm;
        if ((vec_shm = shmat(vec_shm_id, NULL, 0)) == (double*)-1) {
                fprintf(stderr, "shmat failed\n");
                goto out_dt_result;
        };

        for (int i = 0; i < veclen; i++) {
                fscanf(f, "%lf\n", &vec_shm[i]);
        }

        int segment_size = ceil(veclen / (double)N);
        for (int i = 0; i < N; i++) {
                range_shm[i * 2] = segment_size * i;
                range_shm[i * 2 + 1] = MIN((i + 1) * segment_size - 1, veclen - 1);
        }

        char r;
        for (int i = 0; i < N; i++) {
                read(pd[0], &r, 1);
        }

        for (int i = 0; i < N; i++) {
                kill(children[i], SIGUSR1);
        }

        for (int i = 0; i < N; i++) {
                if (wait(0) == -1) {
                        fprintf(stderr, "wait failed\n");
                        goto out_close_pipe;
                }
        }

        double total = 0.0;
        for (int i = 0; i < N; i++) {
                total += result_shm[i];
        }

        printf("total is %lf\n", total);

        ret = EXIT_SUCCESS;

out_close_pipe:
        close(pd[0]);
        close(pd[1]);

//out_dt_vec:
        shmdt(vec_shm);
out_dt_result:
        shmdt(result_shm);
out_dt_range:
        shmdt(range_shm);

out_rm_vec_shm:
        shmctl(vec_shm_id, IPC_RMID, 0);
out_rm_result_shm:
        shmctl(result_shm_id, IPC_RMID, 0);
out_rm_range_shm:
        shmctl(range_shm_id, IPC_RMID, 0);

out_close_file:
        fclose(f);

out_free_children:
        free(children);

        return ret;
}
