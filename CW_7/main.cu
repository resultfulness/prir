%%cuda
#include <stdio.h>
#include <math.h>
#include <assert.h>

#define N 10000000

__global__ void vector_add(float *out, float *a, float *b, int n) {
    int posinblock = threadIdx.x;
    int ninblock = blockDim.x;

    int posofblock = blockIdx.x;
    int nblocks = gridDim.x;

    int per_block = N / nblocks;

    for (int i = posinblock + per_block * posofblock;
         i < (posofblock+1) * per_block; 
         i += ninblock) {
        out[i] = a[i] + b[i];
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

