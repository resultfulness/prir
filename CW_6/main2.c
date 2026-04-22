#include <stdio.h>
#include <mpi.h>

#define LEFT 0
#define RIGHT 1

#define TAG 1
#define CHUNK_SIZE 1000
#define VEC_SIZE (int)1e6

void read_chunk(int *src, int *dst) {
    for (int i = 0; i < CHUNK_SIZE; i++) {
        dst[i] = src[i];
    }
}

int main(int argc, char **argv) {
    int num_processes, world_rank, cart_rank;
    int neighbours[2];
    MPI_Comm cart_comm;
    MPI_Status status;

    int dims[1] = { 4 };
    int periods[1] = { 0 };
    int vec[VEC_SIZE] = { 0 };

    for (int i = 0; i < VEC_SIZE; i++) {
        vec[i] = i;
    }

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, 0, &cart_comm);

    MPI_Comm_rank(cart_comm, &cart_rank);
    MPI_Cart_shift(cart_comm, 0, 1, &neighbours[LEFT], &neighbours[RIGHT]);

    printf(
        "[#%d] neighbours=[ %d %d ]\n",
        cart_rank, neighbours[LEFT], neighbours[RIGHT]
    );

    int vec_buf[CHUNK_SIZE];

    for (int i = 0; i < VEC_SIZE / CHUNK_SIZE; i++) {
        if (world_rank == 0) {
            read_chunk((int *)(vec + i * 1000), (int *)&vec_buf);
        }

        MPI_Recv(
            &vec_buf, CHUNK_SIZE, MPI_INT,
            neighbours[LEFT],
            TAG, cart_comm, &status
        );

        if (status.MPI_SOURCE != MPI_PROC_NULL) printf(
            "[#%d] got chunk from #%d: "
            "vec_buf[0]=%d; vec_buf[CHUNK_SIZE - 1]=%d\n",
            cart_rank, neighbours[LEFT], vec_buf[0], vec_buf[CHUNK_SIZE - 1]
        );

        MPI_Send(
            &vec_buf, CHUNK_SIZE, MPI_INT,
            neighbours[RIGHT],
            TAG, cart_comm
        );

        printf("[#%d] sent chunk to #%d\n", cart_rank, neighbours[RIGHT]);
    }

    printf("[#%d] finished\n", cart_rank);

    MPI_Finalize();

    return 0;
}
