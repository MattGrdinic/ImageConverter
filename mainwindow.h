#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>

#include "imagemanager.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots :
    void slotFileSelect();
    void slotProcessImages();
    void slotThreadMessage(QString);
    void slotThreadsFinsihed();

    void on_pushButton_clearDisplay_clicked();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

    QString folder;

    QTime startTime;
    QTime endTime;

    ImageManager *im;
};

#endif // MAINWINDOW_H
