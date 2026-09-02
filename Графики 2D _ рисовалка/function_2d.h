#ifndef FUNCTION_2D_H
#define FUNCTION_2D_H

class Function2D {
public:
    Function2D(int k = 0);
    void setK(int k);
    double eval(double x, double y) const;
    double eval_fx(double x, double y) const;
    double eval_fy(double x, double y) const;
    double eval_fxy(double x, double y) const;
    const char* name() const;
    int getK() const { return k_; }
private:
    int k_;
};

#endif
