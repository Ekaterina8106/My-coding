#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include <QWidget>
#include <vector>
#include <functional>

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    GraphWidget(QWidget *parent, double a, double b, int n, int k);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    public slots:   // <-- изменено с private slots на public slots
        void changeFunction(); // вызывается из меню

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setFunctionK(int k);
    void updateNodesAndApprox();
    void recomputeBounds();
    QPointF graphToWindow(double x_graph, double y_graph);
    double computeMaxAbsF();

    double m_a_original;
    double m_b_original;
    double m_center;
    int m_n;
    int m_k;
    int m_s;
    int m_p;
    int m_displayMode;

    double m_a_disp;
    double m_b_disp;
    double m_min_y;
    double m_max_y;

    std::vector<double> m_x_nodes;
    std::vector<double> m_f_nodes;
    std::vector<double> m_df_nodes;
    double m_max_abs_f;

    std::vector<double> m_coeff1;
    std::vector<double> m_coeff2;
    bool m_valid1;
    bool m_valid2;

    std::function<double(double)> m_func;
    std::function<double(double)> m_deriv;
    const char *m_func_name;

    static const int PLOT_RESOLUTION = 1000;
};

#endif
