#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int rand_int()
{
    int upper = 1000000;
    return rand() % upper + 1;
}

int gcd(int a, int b)
{
    while (a != b) {
        if (a > b)
            a = a - b;
        else
            b = b - a;
    }
    return a;
}

int calculate(int num_processes, int world_rank)
{
    int own = rand_int();
    printf("[%d] starts with %d\n", world_rank, own);
    MPI_Status status;
    for (int i = 1; i <= log2(num_processes); i++) {
        int received;
        MPI_Send(&own, 1, MPI_INT, (world_rank + i) % num_processes, 0, MPI_COMM_WORLD);
        MPI_Recv(&received, 1, MPI_INT, (world_rank - i + num_processes) % num_processes, 0, MPI_COMM_WORLD, &status);
        own = gcd(own, received);
    }
    return own;
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    int num_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    srand(time(0) + world_rank);
    int r;
    if (argc > 1) {
        while ((r = calculate(num_processes, world_rank)) == 1) {
            if (world_rank == 0) {
                printf("Nowa runda !\n");
            }
            usleep(2000);
        }
        usleep(2000);
        if (world_rank == 0) {
            printf("%d\n", r);
        }
    } else {
        r = calculate(num_processes, world_rank);
        usleep(2000);
        if (world_rank == 0) {
            printf("%d\n", r);
        }
    }
    return 0;
}
