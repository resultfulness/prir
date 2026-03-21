#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

struct matrix {
    int rows;
    int cols;
    double** vals;
};

int N;
struct matrix* A;
struct matrix* B;
struct matrix* R;

pthread_mutex_t sum_mutex;
double global_sum = 0;

struct matrix* load_matrix(char* filename)
{
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return NULL;
    }

    struct matrix* m = malloc(sizeof(struct matrix));
    if (m == NULL) {
        goto out_close_file;
    }

    fscanf(f, "%d\n", &m->rows);
    fscanf(f, "%d\n", &m->cols);
    if (m->rows <= 0 || m->cols <= 0) {
        fprintf(stderr, "Invalid number of rows or columns declared\n");
        goto out_free_matrix;
    }

    m->vals = malloc(sizeof(double*) * m->rows);
    if (m->vals == NULL) {
        goto out_free_matrix;
    }

    int error_row;
    for (int i = 0; i < m->rows; i++) {
        m->vals[i] = malloc(sizeof(double) * m->cols);

        if (m->vals[i] == NULL) {
            error_row = i;
            goto out_free_rows;
        }

        for (int j = 0; j < m->cols; j++) {
            fscanf(f, "%lf", &m->vals[i][j]);
        }
    }

    goto out_close_file;

out_free_rows:
    for (int i = 0; i < error_row; i++) {
        free(m->vals[i]);
    }
out_free_matrix:
    free(m->vals);
    free(m);
    m = NULL;
out_close_file:
    fclose(f);
    return m;
}

struct matrix* create_empty_matrix(int r, int c)
{
    struct matrix* m = malloc(sizeof(struct matrix));
    if (m == NULL) {
        goto out_return;
    }

    m->cols = c;
    m->rows = r;

    m->vals = malloc(sizeof(double*) * m->rows);
    if (m->vals == NULL) {
        goto out_free_matrix;
    }

    int error_row;
    for (int i = 0; i < m->rows; i++) {
        m->vals[i] = calloc(m->cols, sizeof(double));

        if (m->vals[i] == NULL) {
            error_row = i;
            goto out_free_rows;
        }
    }
    goto out_return;

out_free_rows:
    for (int i = 0; i < error_row; i++) {
        free(m->vals[i]);
    }
out_free_matrix:
    free(m->vals);
    free(m);
    m = NULL;
out_return:
    return m;
}

void free_matrix(struct matrix* m)
{
    for (int i = 0; i < m->rows; i++) {
        free(m->vals[i]);
    }
    free(m->vals);
    free(m);
}

void print_matrix(struct matrix m)
{
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            printf("%lf ", m.vals[i][j]);
        }
        printf("\n");
    }
}

void* run(void* arg)
{
    int index = *(int*)arg;
    int tmp = index;
    while (tmp < R->cols * R->rows) {
        double sum = 0;
        int c = tmp % R->cols;
        int r = tmp / R->cols;
        for (int i = 0; i < A->cols; i++) {
            sum += A->vals[r][i] * B->vals[i][c];
        }
        R->vals[r][c] = sum;

        pthread_mutex_lock(&sum_mutex);
        global_sum += sum * sum;
        pthread_mutex_unlock(&sum_mutex);
        tmp += N;
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    int exit_status = EXIT_FAILURE;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <n threads> <filename1> <filename2>\n", argv[0]);
        goto out_exit;
    }

    N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "Thread number must me positive\n");
        goto out_exit;
    }

    char* filename1 = argv[2];
    char* filename2 = argv[3];

    A = load_matrix(filename1);
    if (A == NULL) {
        fprintf(stderr, "Errod loading matrix: %s\n", filename1);
        goto out_exit;
    }
    B = load_matrix(filename2);
    if (B == NULL) {
        fprintf(stderr, "Errod loading matrix: %s\n", filename2);
        goto out_free_A;
    }

    R = create_empty_matrix(A->rows, B->cols);
    if (R == NULL) {
        fprintf(stderr, "Errod creating matrix R\n");
        goto out_free_B;
    }

    pthread_mutex_init(&sum_mutex, NULL);
    pthread_t* threads = malloc(sizeof(pthread_t) * N);
    if (threads == NULL) {
        goto out_free_R;
    }

    int* indexes = malloc(sizeof(int) * N);
    if (indexes == NULL) {
        goto out_free_threads;
    }

    for (int i = 0; i < N; i++) {
        indexes[i] = i;
        if(pthread_create(&threads[i], NULL, &run, &indexes[i]) != 0) {
            fprintf(stderr, "Thread %d failed to initialize", i);
            goto out_free_indexes;
        }
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Result matrix:\n");
    print_matrix(*R);
    printf("Frobenius norm: %lf\n", sqrt(global_sum));

    exit_status = EXIT_SUCCESS;

out_free_indexes:
    free(indexes);
out_free_threads:
    free(threads);
out_free_R:
    free_matrix(R);
out_free_B:
    free_matrix(B);
out_free_A:
    free_matrix(A);
out_exit:
    return exit_status;
}
