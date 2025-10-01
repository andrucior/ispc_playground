// vector_add.ispc
export void vector_add(uniform float *a, uniform float *b, uniform float *c, uniform int n) {
    foreach (i = 0 ... n) {
        c[i] = a[i] + b[i];
    }
}