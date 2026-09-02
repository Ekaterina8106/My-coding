#pragma once

double** allocate_matrix(int n, int m);
void free_matrix(double** matrix, int n);
void init_matrix(int n, double** A, int s, char* filename);

