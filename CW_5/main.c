#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double my_sin(double x) {
    return sin(x);
}

double integrate(
    double (*func)(double),
    double begin,
    double end,
    double num_points
) {
    if (num_points == 0) {
        return
            (end - begin) / 6 *
            (func(begin) + 4 * func((begin + end) / 2) + func(end));
    }

    double chunk_size = (end - begin) / num_points;
    double total = 0;

    for (double i = begin; i < end; i += chunk_size) {
        double j = i + chunk_size;
        total +=
            (j - i) / 6 * (func(i) + 4 * func((i + j) / 2) + func(j)); 
    }

    return total;
}

double *split_work(
    double begin,
    double end,
    int num_points,
    int num_processes
) {
    double *res = malloc(sizeof(double) * num_processes * 3);

    int min_points_per_process = num_points / num_processes;
    int remainder = num_points % num_processes;

    double chunk_size = (end - begin) / num_processes;

    for (int i = 0; i < num_processes; i++) {
        res[i * 3] = i * chunk_size;
        res[i * 3 + 1] = (i + 1) * chunk_size;
        res[i * 3 + 2] = min_points_per_process;
        if (remainder > 0) {
            res[i * 3 + 2]++;
            remainder--;
        }
        // printf("process %d got res[3i+0] = %lf\n", i, res[3*i+0]);
        // printf("process %d got res[3i+1] = %lf\n", i, res[3*i+1]);
        // printf("process %d got res[3i+2] = %lf\n", i, res[3*i+2]);
    }

    return res;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        return 1;
    }

    double begin = atof(argv[1]);
    double end = atof(argv[2]);

    if (end < begin) {
        return 1;
    }

    int num_points = atoi(argv[3]);
    if (num_points == 0) {
        return 1;
    }

    MPI_Init(&argc, &argv);

    int num_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    double *params;
    if (world_rank == 0) {
        params = split_work(begin, end, num_points, num_processes);
    }

    double *sub_param = malloc(sizeof(double) * 3);
    double *sub_sums;
    if (world_rank == 0) {
        sub_sums = malloc(sizeof(double) * num_processes);
    }

    MPI_Scatter(
        params,
        3,
        MPI_DOUBLE,
        sub_param,
        3,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD
    );

    double sub_sum = integrate(
        my_sin,
        sub_param[0],
        sub_param[1],
        sub_param[2]
    );

    MPI_Gather(
        &sub_sum,
        1,
        MPI_DOUBLE,
        sub_sums,
        1,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD
    );

    if (world_rank == 0) {
        double sum = 0;

        for (int i = 0; i < num_processes; i++) {
            sum += sub_sums[i];
        }

        printf("%lf\n", sum);

        free(params);
    }

    MPI_Finalize();
    return 0;
}
