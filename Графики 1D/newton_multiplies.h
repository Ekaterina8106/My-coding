#ifndef NEWTON_MULTIPLES_H
#define NEWTON_MULTIPLES_H

// Построение интерполяционного многочлена Ньютона с кратными узлами (кратность 2)
// n      – число исходных узлов
// x      – массив исходных узлов (длины n)
// f      – массив значений функции в узлах (длины n)
// df     – массив значений производной в узлах (длины n)
// coeff  – выходной массив коэффициентов (длины 2*n)
// temp   – временный массив (длины 2*n) для хранения разделенных разностей
void build_newton_multiples(int n, const double *x, const double *f, const double *df,
                            double *coeff, double *temp);

// Вычисление значения многочлена в точке x
// a, b   – границы отрезка (не используются, но передаются для унификации)
// n      – число исходных узлов
// x_nodes – массив исходных узлов (длины n)
// coeff  – массив коэффициентов (длины 2*n), полученный build_newton_multiples
double newton_multiples_eval(double x, double a, double b, int n,
                             const double *x_nodes, const double *coeff);

#endif
