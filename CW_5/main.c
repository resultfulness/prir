#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double my_sin(double x) {
    return sin(x);
}

double simpson_rule(double (*func)(double), double begin, double end) {
    return (end - begin) / 6 * (
        func(begin) +
        4 * func((begin + end) / 2) +
        func(end)
    );
}

double integrate(
    double (*func)(double),
    double begin,
    double end,
    double num_points
) {
    if (num_points == 0) {
        return simpson_rule(func, begin, end);
    }

    double chunk_size = (end - begin) / num_points;
    double total = 0;

    for (double i = begin; i < end; i += chunk_size) {
        double j = i + chunk_size;
        total += simpson_rule(func, i, j);
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
    if (res == NULL) {
        return NULL;
    }

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
    MPI_Init(&argc, &argv);

    int num_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int ret = 1;

    if (argc < 4) {
        if (world_rank == 0) fprintf(
            stderr,
            "Usage: %s <interval start> <interval end> <num points>\n",
            argv[0]
        );
        goto out_early_err;
    }

    double begin = atof(argv[1]);
    double end = atof(argv[2]);

    if (end < begin) {
        if (world_rank == 0) fprintf(
            stderr,
            "end of interval before start of interval!\n"
        );
        goto out_early_err;
    }

    int num_points = atoi(argv[3]);
    if (num_points == 0) {
        if (world_rank == 0) fprintf(
            stderr,
            "num points must be a positive integer"
        );
        goto out_early_err;
    }

    double *params, *sub_sums;
    if (world_rank == 0) {
        params = split_work(begin, end, num_points, num_processes);
        sub_sums = malloc(sizeof(double) * num_processes);
        if (params == NULL || sub_sums == NULL) {
            fprintf(stderr, "malloc error\n");
            goto out_free;
        }
    }

    double *sub_param = malloc(sizeof(double) * 3);
    if (sub_param == NULL) {
        fprintf(stderr, "malloc error\n");
        goto out_free;
    }

    MPI_Scatter(
        params, 3, MPI_DOUBLE,
        sub_param, 3, MPI_DOUBLE,
        0, MPI_COMM_WORLD
    );

    double sub_sum = integrate(
        my_sin,
        sub_param[0],
        sub_param[1],
        sub_param[2]
    );

    MPI_Gather(
        &sub_sum, 1, MPI_DOUBLE,
        sub_sums, 1, MPI_DOUBLE,
        0, MPI_COMM_WORLD
    );

    if (world_rank == 0) {
        double sum = 0;
        for (int i = 0; i < num_processes; i++) {
            sum += sub_sums[i];
        }

        printf("integration result: %lf\n", sum);
    }

    ret = 0;

out_free:
    if (world_rank == 0) {
        if (params) free(params);
        if (sub_sums) free(sub_sums);
    }
    if (sub_param) free(sub_param);

    MPI_Finalize();

out_early_err:
    return ret;
}
