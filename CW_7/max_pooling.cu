%% cuda
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

__device__ float max_block(
    float *in,
    int in_width,
    int in_height,
    int out_width,
    int out_height,
    int mask_size,
    int i
) {
    float max = 0;
    int o_x = i % out_width;
    int o_y = i / out_width;
    for (int j = 0; j < mask_size * mask_size; j++) {
        int i_x = o_x * mask_size + j % mask_size;
        int i_y = o_y * mask_size + j / mask_size;
        if(i_x >= in_width) continue;
        if(i_y >= in_height) continue;
        max = fmax(max, in[i_x + in_width * i_y]);
    }
    return max;
}

__global__ void max_pool(
    float *in,
    float *out,
    int in_width,
    int in_height,
    int out_width,
    int out_height,
    int mask_size
) {
    int out_size = out_width * out_height;

    int thread_i = blockDim.x * blockIdx.x + threadIdx.x;
    int n_threads = gridDim.x * blockDim.x;
    int per_thread = out_size / n_threads;
    int thread_chunk_start = thread_i * per_thread;

    for (int i = thread_chunk_start; i < thread_chunk_start + per_thread; i++) {
        out[i] = max_block(in, in_width, in_height, out_width, out_height, mask_size, i);
    }

    int remainder = out_size % n_threads;

    if (thread_i < remainder) {
        int extra_chunk_start = per_thread * n_threads;
        int extra_chunk_i = extra_chunk_start + thread_i;
        out[extra_chunk_i] = max_block(in, in_width, in_height, out_width, out_height, mask_size, extra_chunk_i);
    }
}

int main()
{
    int in_width, in_height;
    int out_width, out_height;
    int mask_size;
    int in_size, out_size;
    float *in, *out;
    float *dev_in, *dev_out;

    FILE* fmask = fopen("maska.dat", "r");
    fscanf(fmask, "%d", &mask_size);
    fclose(fmask);

    FILE* fmatrix = fopen("macierz.dat", "r");
    fscanf(fmatrix, "%d", &in_width);
    fscanf(fmatrix, "%d", &in_height);
    in_size = in_width * in_height;

    in = malloc(sizeof(float) * in_size);

    float x;
    for (int i = 0; i < in_width * in_height; i++) {
        fscanf(fmatrix, "%f\n", &x);
        in[i] = x;
    }
    fclose(fmatrix);

    out_width = (in_width + mask_size - 1) / mask_size;
    out_height = (in_height + mask_size - 1) / mask_size;
    out_size = out_width * out_height;
    out = malloc(sizeof(float) * out_size);

    cudaMalloc(&dev_in, sizeof(float) * in_size);
    cudaMalloc(&dev_out, sizeof(float) * out_size);

    cudaMemcpy(dev_in, in, sizeof(float) * in_size, cudaMemcpyHostToDevice);

    max_pool<<<1, 1>>>(dev_in, dev_out, in_width, in_height, out_width, out_height, mask_size);

    cudaMemcpy(out, dev_out, sizeof(float) * out_size, cudaMemcpyDeviceToHost);

    for (int i = 0; i < out_size; i++) {
        printf("%f\n", out[i]);
    }

    cudaFree(dev_in);
    cudaFree(dev_out);

    free(in);
    free(out);
}
