#include "functions.h"
#include <cmath>

double func(int k, double x) {
    switch (k) {
        case 0: return 1.0;
        case 1: return x;
        case 2: return x * x;
        case 3: return x * x * x;
        case 4: return x * x * x * x;
        case 5: return std::exp(x);
        case 6: return 1.0 / (25.0 * x * x + 1.0);
        default: return 0.0;
    }
}

double func_deriv(int k, double x) {
    switch (k) {
        case 0: return 0.0;
        case 1: return 1.0;
        case 2: return 2.0 * x;
        case 3: return 3.0 * x * x;
        case 4: return 4.0 * x * x * x;
        case 5: return std::exp(x);
        case 6: {
            double denom = 25.0 * x * x + 1.0;
            return -50.0 * x / (denom * denom);
        }
        default: return 0.0;
    }
}

const char* func_name(int k) {
    switch (k) {
        case 0: return "f(x)=1";
        case 1: return "f(x)=x";
        case 2: return "f(x)=x^2";
        case 3: return "f(x)=x^3";
        case 4: return "f(x)=x^4";
        case 5: return "f(x)=exp(x)";
        case 6: return "f(x)=1/(25x^2+1)";
        default: return "unknown";
    }
}
