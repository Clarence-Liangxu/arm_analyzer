#include <cstdint>
#include <cstring>

void test_add(int32_t *a, int32_t *b, int32_t *c) {
    for (int i = 0; i < 4; i++) {
        a[i] = b[i] + c[i];
    }
}

void test_sub(int32_t *a, int32_t *b, int32_t *c) {
    for (int i = 0; i < 4; i++) {
        a[i] = b[i] - c[i];
    }
}

void test_mul(int32_t *a, int32_t *b, int32_t *c) {
    for (int i = 0; i < 4; i++) {
        a[i] = b[i] * c[i];
    }
}

void test_dot(int32_t *a, int32_t *b, int32_t *c) {
    for (int i = 0; i < 4; i++) {
        a[0] += b[i] * c[i];
    }
}

void test_float(float *a, float *b, float *c) {
    for (int i = 0; i < 4; i++) {
        a[i] = b[i] + c[i];
    }
}

void test_uint8(uint8_t *a, uint8_t *b, uint8_t *c) {
    for (int i = 0; i < 16; i++) {
        a[i] = b[i] + c[i];
    }
}

void test_uint16(uint16_t *a, uint16_t *b, uint16_t *c) {
    for (int i = 0; i < 8; i++) {
        a[i] = b[i] + c[i];
    }
}
