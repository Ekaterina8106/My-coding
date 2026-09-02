#ifndef HERMITE_PIECEWISE_H
#define HERMITE_PIECEWISE_H

// Построение кусочно-кубической функции Эрмита
// n      – число узлов
// x      – массив узлов (длины n)
// f      – массив значений функции в узлах (длины n)
// df     – массив значений производной в узлах (длины n)
// coeff  – выходной массив коэффициентов (длины 2*n): первые n – f, вторые n – df
// temp   – не используется (может быть nullptr)
void build_hermite(int n, const double *x, const double *f, const double *df,
                   double *coeff, double * /*temp*/);

// Вычисление значения кусочно-кубической функции в точке x
// a, b   – границы отрезка (используются для экстраполяции за пределы)
// n      – число узлов
// x_nodes – массив узлов (длины n)
// coeff  – массив коэффициентов (длины 2*n): первые n – f, вторые n – df
double hermite_eval(double x, double a, double b, int n,
                    const double *x_nodes, const double *coeff);

#endif
