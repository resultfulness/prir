%%cuda
#include <stdio.h>
#include <math.h>
#include <assert.h>

#define N 10000000

__global__ void vector_add(float *out, float *a, float *b, int n) {
    int thread_i = blockDim.x * blockIdx.x + threadIdx.x;
    int n_threads = gridDim.x * blockDim.x;
    int per_thread = N / n_threads;
    int thread_chunk_start = thread_i * per_thread;

    for (int i = thread_chunk_start; i < thread_chunk_start + per_thread; i++) {
        out[i] = a[i] + b[i];
    }

    int remainder = N % n_threads;

    if (thread_i < remainder) {
        int extra_chunk_start = per_thread * n_threads;
        int extra_chunk_i = extra_chunk_start + thread_i;
        out[extra_chunk_i] = a[extra_chunk_i] + b[extra_chunk_i];
    }
}

int main(){
    float *a, *b, *out;
    float *dev_a, *dev_b, *dev_out;

    cudaMalloc(&dev_a, sizeof(float) * N);
    cudaMalloc(&dev_b, sizeof(float) * N);
    cudaMalloc(&dev_out, sizeof(float) * N);

    a   = (float*)malloc(sizeof(float) * N);
    b   = (float*)malloc(sizeof(float) * N);
    out = (float*)malloc(sizeof(float) * N);

    for(int i = 0; i < N; i++){
        a[i] = 1.0f; b[i] = 2.0f;
    }

    cudaMemcpy(dev_a, a, sizeof(float) * N, cudaMemcpyHostToDevice);
    cudaMemcpy(dev_b, b, sizeof(float) * N, cudaMemcpyHostToDevice);

    //vector_add<<<1, 1>>>(dev_out, dev_a, dev_b, N);
    //vector_add<<<1, 256>>>(dev_out, dev_a, dev_b, N);
    vector_add<<<16, 16>>>(dev_out, dev_a, dev_b, N);

    cudaMemcpy(out, dev_out, sizeof(float) * N, cudaMemcpyDeviceToHost);

    for (int i = 0; i< N; i++) {
        assert(fabs(out[i] - a[i] - b[i]) < 1e-6);
        //printf("%f\n", out[i]);
    }

    printf("PAssed\n");

    cudaFree(dev_a);
    cudaFree(dev_b);
    cudaFree(dev_out);

    free(a);
    free(b);
    free(out);
}

