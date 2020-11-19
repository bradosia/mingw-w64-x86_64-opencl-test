void kernel simple_add(global int *A, global int *B, global int *C,
                       const int n) {
  for (int i = 0; i < n; i++) {
    C[i] = A[i] + B[i];
  }
}
