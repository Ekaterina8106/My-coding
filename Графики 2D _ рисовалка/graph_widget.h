#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include <QOpenGLWidget>
#include "function_2d.h"
#include "newton_multiples_2d.h"
#include "cubic_hermite_piecewise_2d.h"

class GraphWidget : public QWidget {
    Q_OBJECT
public:
    GraphWidget(QWidget* parent, double a, double b, double c, double d,
                int nx, int ny, int mx, int my, int k);
    ~GraphWidget();
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* e) override;
private:
    void updateApproximations();
    void recomputeBounds();
    void recomputeValues();
    
    QPointF project(double x, double y, double z) const;

    double a_, b_, c_, d_;
    int nx_, ny_, mx_, my_;
    int k_, p_, s_, drawMode_;
    double angle_; // угол поворота вокруг OZ (градусы)
    Function2D func_;
    NewtonMultiples2D method1_;
    CubicHermitePiecewise2D method2_;
    double max_abs_f_;
    double minX_, maxX_, minY_, maxY_;
    double minZ_, maxZ_, maxAbsCurrent_;
    double* zValues_;
    bool needRebuild_;
    
    double dist_;
    int lastWidth_, lastHeight_;
};

#endif
