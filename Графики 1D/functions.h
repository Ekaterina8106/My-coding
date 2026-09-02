#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Возвращает значение функции f_k(x) для k = 0..6
double func(int k, double x);

// Возвращает значение производной f'_k(x) для k = 0..6
double func_deriv(int k, double x);

// Возвращает имя функции в виде строки
const char* func_name(int k);

#endif
