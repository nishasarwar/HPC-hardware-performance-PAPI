#include <assert.h>
#include <stdlib.h>
#include <papi.h>

__attribute__((noipa))
void do_not_optimize(float *_) {}

int main(int argc, char** argv)
{
    assert(argc == 4);

    PAPI_library_init(PAPI_VER_CURRENT);

    int M = atoi(argv[1]);
    int N = atoi(argv[2]);
    int O = atoi(argv[3]);

    float *A = (float*)calloc(M * O, sizeof(float));
    float *B = (float*)calloc(O * N, sizeof(float));
    float *C = (float*)calloc(M * N, sizeof(float));

    PAPI_hl_region_begin("initialize");

    for (int i = 0; i < M * O; ++i) {
        A[i] = 1./(rand() % 1024 + 1);
    }

    for (int i = 0; i < O * N; ++i) {
        B[i] = 1./(rand() % 1024 + 1);
    }

    PAPI_hl_region_end("initialize");

    PAPI_hl_region_begin("multiply");

    // IMPLEMENTING MATRIX-MATRIX MULTIPLY HERE
    
    for (int i = 0; i < M; ++i){
        for (int j = 0; j < N; ++j){
            for (int k = 0; k < O; ++k){
                C[i*N + j] += A[i*O + k] * B[k*N + j];
            }
        }
    }

    PAPI_hl_region_end("multiply");

    do_not_optimize(C);

    free(C);
    free(B);
    free(A);

    return 0;
}
