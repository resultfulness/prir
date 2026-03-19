#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct matrix {
    int rows;
    int cols;
    double **vals;
};

struct matrix* A;
struct matrix* B;
struct matrix* R;

pthread_mutex_t sum_mutex;
double global_sum = 0;
int N;

struct matrix* load_matrix(char* filename) {
    struct matrix* m = malloc(sizeof(struct matrix));
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        return NULL;
    }
    fscanf(f, "%d\n", &m->rows);
    fscanf(f, "%d\n", &m->cols);
    if (m->rows <= 0 || m->cols <= 0) {
        return NULL;
    }
    m->vals = malloc(sizeof(double *) * m->rows);
    for (int i = 0; i < m->rows; i++) {
        m->vals[i] = malloc(sizeof(double) * m->cols);
        for (int j = 0; j < m->cols; j++) {
            fscanf(f, "%lf", &m->vals[i][j]);
        }
    }
    return m;
}

struct matrix* create_empty_matrix(int r, int c) {
    struct matrix* m = malloc(sizeof(struct matrix));
    m->cols = c;
    m->rows = r;
    m->vals = malloc(sizeof(double *) * m->rows);
    for (int i = 0; i < m->rows; i++) {
        m->vals[i] = calloc(m->cols, sizeof(double));
    }
    return m;
}

void print_matrix(struct matrix m) {
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j<m.cols; j++) {
            printf("%lf ", m.vals[i][j]);
        }
        printf("\n");
    }
}

void* run(void* arg) {
    int index = *(int*)arg;
    int tmp = index;
    while (tmp < R->cols * R->rows) {
        double sum = 0;
        int c = tmp % 3;
        int r = tmp / 3;
        for (int i = 0; i < A->cols; i++) {
            sum += A->vals[r][i] * B->vals[i][c];
        }
        R->vals[r][c] = sum;
        pthread_mutex_lock(&sum_mutex);
        global_sum += sum;
        pthread_mutex_unlock(&sum_mutex);
        tmp+=N;
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <n threads> <filename1> <filename2>\n", argv[0]);
        return EXIT_FAILURE;
    }

    N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "Thread number must me positive\n");
        return EXIT_FAILURE;
    }

    char *filename1 = argv[2];
    char *filename2 = argv[3];

    A = load_matrix(filename1);
    if (A == NULL) {
        fprintf(stderr, "Errod loading matrix: %s\n", filename1);
    }
    B = load_matrix(filename2);
    if (B == NULL) {
        fprintf(stderr, "Errod loading matrix: %s\n", filename2);
    }


    R = create_empty_matrix(A->rows, B->cols);
    int* indexes = malloc(sizeof(int) * N);
    pthread_t* threads = malloc(sizeof(pthread_t) * N);

    pthread_mutex_init(&sum_mutex, NULL);
    for (int i = 0; i < N; i++) {
        indexes[i] = i;
        pthread_create(&threads[i], NULL, &run, &indexes[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    print_matrix(*R);

    return EXIT_SUCCESS;
}
