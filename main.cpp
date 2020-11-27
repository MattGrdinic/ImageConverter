#include <QApplication>
#include "mainwindow.h"

int t()
{
    int *bits[2];
    *bits[0] = 0; *bits[1] = 1;
    int cmyk_temp[2];
    int count = 2;

    for(int i = 0; i < count; i++){
        cmyk_temp[i] = *bits[i];
    }

    return cmyk_temp[1];

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //int y = t();

    MainWindow w;
    w.show();
    return a.exec();
}
