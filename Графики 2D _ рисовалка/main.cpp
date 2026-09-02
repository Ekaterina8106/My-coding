#include <QApplication>
#include <QMainWindow>
#include <cstdio>
#include "graph_widget.h"

int main(int argc, char** argv) {
    double a=-5.5, b=5.5, c=-5.5, d=5.5;
    int nx=10, ny=10, mx=50, my=50, k=0;
    if (argc == 10) {
        sscanf(argv[1],"%lf",&a); sscanf(argv[2],"%lf",&b);
        sscanf(argv[3],"%lf",&c); sscanf(argv[4],"%lf",&d);
        sscanf(argv[5],"%d",&nx); sscanf(argv[6],"%d",&ny);
        sscanf(argv[7],"%d",&mx); sscanf(argv[8],"%d",&my);
        sscanf(argv[9],"%d",&k);
    } else if (argc != 1) {
        fprintf(stderr,"Usage: %s a b c d nx ny mx my k\n",argv[0]);
        return -1;
    }
    QApplication app(argc, argv);
    QMainWindow win;
    GraphWidget* w = new GraphWidget(&win, a,b,c,d,nx,ny,mx,my,k);
    win.setCentralWidget(w);
    win.setWindowTitle("3D Approximation of 2D functions");
    win.show();
    return app.exec();
}
