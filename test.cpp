#include <cstdint>

void add_arrays(int32_t *dst, const int32_t *src1, const int32_t *src2) {
    for (int i = 0; i < 4; i++) {
        dst[i] = src1[i] + src2[i];
    }
}
