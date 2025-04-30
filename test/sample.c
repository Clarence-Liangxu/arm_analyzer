#include <math.h>

void test_add(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i]; // add
    }
}

void test_sub(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] - b[i]; // sub
    }
}

void test_mul(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] * b[i]; // mul
    }
}

void test_div(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] / b[i]; // div
    }
}

void test_fma(float *a, float *b, float *c, float *d, int n) {
    for (int i = 0; i < n; i++) {
        d[i] = fmaf(a[i], b[i], c[i]); // fma
    }
}

void test_fabs(float *a, float *b, int n) {
    for (int i = 0; i < n; i++) {
        b[i] = fabsf(a[i]); // fabs
    }
}

void test_neg(float *a, float *b, int n) {
    for (int i = 0; i < n; i++) {
        b[i] = -a[i]; // neg
    }
}
