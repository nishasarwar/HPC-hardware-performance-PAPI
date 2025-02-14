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

    // 1. Loop Order: IJK
    PAPI_hl_region_begin("loop ijk");
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < O; ++k) {
                C[j * M + i] += A[k * M + i] * B[j * O + k];
            }
        }
    }
    PAPI_hl_region_end("loop ijk");

    // 2. Loop Order: IKJ
    PAPI_hl_region_begin("loop ikj");
    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < O; ++k) {
            for (int j = 0; j < N; ++j) {
                C[j * M + i] += A[k * M + i] * B[j * O + k];
            }
        }
    }
    PAPI_hl_region_end("loop ikj");

    // 3. Loop Order: JIK
    PAPI_hl_region_begin("loop jik");
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < M; ++i) {
            for (int k = 0; k < O; ++k) {
                C[j * M + i] += A[k * M + i] * B[j * O + k];
            }
        }
    }
    PAPI_hl_region_end("loop jik");

    // 4. Loop Order: JKI
    PAPI_hl_region_begin("loop jki");
    for (int j = 0; j < N; ++j) {
        for (int k = 0; k < O; ++k) {
            for (int i = 0; i < M; ++i) {
                C[j * M + i] += A[k * M + i] * B[j * O + k];
            }
        }
    }
    PAPI_hl_region_end("loop jki");

    // 5. Loop Order: KIJ
    PAPI_hl_region_begin("loop kij");
    for (int k = 0; k < O; ++k) {
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                C[j * M + i] += A[k * M + i] * B[j * O + k];
            }
        }
    }
    PAPI_hl_region_end("loop kij");

    // 6. Loop Order: KJI
    PAPI_hl_region_begin("loop kji");
    for (int k = 0; k < O; ++k) {
        for (int j = 0; j < N; ++j) {
            for (int i = 0; i < M; ++i) {
                C[j * M + i] += A[k * M + i] * B[j * O + k];
            }
        }
    }
    PAPI_hl_region_end("loop kji");

    do_not_optimize(C);

    free(C);
    free(B);
    free(A);

    return 0;
}
