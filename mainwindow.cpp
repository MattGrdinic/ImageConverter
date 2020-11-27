#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //ui->pushButtonStart->setEnabled(false);

    connect(ui->pushButtonSelect, SIGNAL(clicked()),
            this, SLOT(slotFileSelect()));

    connect(ui->pushButtonStart, SIGNAL(clicked()),
            this, SLOT(slotProcessImages()));

    im = new ImageManager;

    connect(im, SIGNAL(threadMessage(QString)),
            this, SLOT(slotThreadMessage(QString)));

    connect(im, SIGNAL(threadsFinsihed()),
            this, SLOT(slotThreadsFinsihed()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::slotFileSelect()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                     "/home/matthew/Documents/image-to-convert",
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    ui->lineEditFolder->setText(dir);

    if(dir != "")
        ui->pushButtonStart->setEnabled(true);

}

void MainWindow::slotProcessImages()
{
    if(ui->lineEditFolder->text() != ""){
        im->processImagesThreaded(ui->lineEditFolder->text(),
                                  ui->comboBox_optimizationLevel->currentIndex(),
                                  ui->comboBox_renderingIntent->currentIndex(),
                                  ui->comboBox_profileInput->currentIndex(),
                                  ui->comboBox_profileOutput->currentIndex());
    }


    startTime = startTime.currentTime();
}

void MainWindow::slotThreadMessage(QString message)
{
    ui->textEditMessages->append(message);
}

void MainWindow::slotThreadsFinsihed()
{

    endTime = endTime.currentTime();

    int elapsedTime = startTime.secsTo(endTime);

    int elapsedTimeMicro = startTime.msecsTo(endTime);

    ui->textEditMessages->append("\n== || Total Process Time Elapsed (seconds): "
                                 + QString::number(elapsedTime));

    ui->textEditMessages->append("== || Total Process Time Elapsed (milliseconds): "
                                 + QString::number(elapsedTimeMicro));
}

void MainWindow::on_pushButton_clearDisplay_clicked()
{
    ui->textEditMessages->clear();
}
