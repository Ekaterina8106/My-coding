#pragma once

void pochti_treyg(int n, double** A);
int lr_algorithm(int n, double** A, double* eigenvalues, double epsilon, int max_iterations);
void compute_residuals(int n, double trace_A, double norm_A_sq, double* eigenvalues, double* residual1, double* residual2);
