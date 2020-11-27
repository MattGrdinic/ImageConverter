/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu May 5 11:55:11 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QComboBox *comboBox_optimizationLevel;
    QTextEdit *textEditMessages;
    QLineEdit *lineEditFolder;
    QLabel *label_2;
    QPushButton *pushButtonSelect;
    QLabel *label;
    QComboBox *comboBox_profileInput;
    QComboBox *comboBox_profileOutput;
    QLabel *label_3;
    QLabel *label_4;
    QPushButton *pushButtonStart;
    QLabel *label_5;
    QComboBox *comboBox_renderingIntent;
    QPushButton *pushButton_clearDisplay;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(469, 613);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        comboBox_optimizationLevel = new QComboBox(centralWidget);
        comboBox_optimizationLevel->setObjectName(QString::fromUtf8("comboBox_optimizationLevel"));
        comboBox_optimizationLevel->setGeometry(QRect(20, 430, 181, 25));
        textEditMessages = new QTextEdit(centralWidget);
        textEditMessages->setObjectName(QString::fromUtf8("textEditMessages"));
        textEditMessages->setGeometry(QRect(21, 76, 421, 251));
        lineEditFolder = new QLineEdit(centralWidget);
        lineEditFolder->setObjectName(QString::fromUtf8("lineEditFolder"));
        lineEditFolder->setGeometry(QRect(120, 40, 321, 25));
        lineEditFolder->setPlaceholderText(QString::fromUtf8(""));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 410, 121, 16));
        pushButtonSelect = new QPushButton(centralWidget);
        pushButtonSelect->setObjectName(QString::fromUtf8("pushButtonSelect"));
        pushButtonSelect->setGeometry(QRect(20, 40, 91, 27));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 150, 20));
        comboBox_profileInput = new QComboBox(centralWidget);
        comboBox_profileInput->setObjectName(QString::fromUtf8("comboBox_profileInput"));
        comboBox_profileInput->setGeometry(QRect(20, 360, 181, 25));
        comboBox_profileOutput = new QComboBox(centralWidget);
        comboBox_profileOutput->setObjectName(QString::fromUtf8("comboBox_profileOutput"));
        comboBox_profileOutput->setGeometry(QRect(260, 360, 181, 25));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 340, 131, 16));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(260, 340, 151, 16));
        pushButtonStart = new QPushButton(centralWidget);
        pushButtonStart->setObjectName(QString::fromUtf8("pushButtonStart"));
        pushButtonStart->setGeometry(QRect(260, 500, 181, 27));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(260, 410, 111, 16));
        comboBox_renderingIntent = new QComboBox(centralWidget);
        comboBox_renderingIntent->setObjectName(QString::fromUtf8("comboBox_renderingIntent"));
        comboBox_renderingIntent->setGeometry(QRect(260, 430, 181, 25));
        pushButton_clearDisplay = new QPushButton(centralWidget);
        pushButton_clearDisplay->setObjectName(QString::fromUtf8("pushButton_clearDisplay"));
        pushButton_clearDisplay->setGeometry(QRect(20, 500, 101, 27));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 469, 23));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        comboBox_optimizationLevel->setCurrentIndex(8);
        comboBox_renderingIntent->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Image Converter", 0, QApplication::UnicodeUTF8));
        comboBox_optimizationLevel->clear();
        comboBox_optimizationLevel->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Base Code", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Optimization 1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Optimization 2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Optimization 3", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Optimization 4", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Optimization 5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "SSE 1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "SSE 2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Raw SSE Assembly", 0, QApplication::UnicodeUTF8)
        );
        lineEditFolder->setInputMask(QString());
        lineEditFolder->setText(QApplication::translate("MainWindow", "/home/matthew/Documents/images-to-convert", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Optimization Level:", 0, QApplication::UnicodeUTF8));
        pushButtonSelect->setText(QApplication::translate("MainWindow", "Select Folder", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Select A Folder To Convert:", 0, QApplication::UnicodeUTF8));
        comboBox_profileInput->clear();
        comboBox_profileInput->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "US Web Uncoated", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "US Web Coated SWOP", 0, QApplication::UnicodeUTF8)
        );
        comboBox_profileOutput->clear();
        comboBox_profileOutput->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Adobe RGB 1998", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Apple RGB", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Color Match RGB", 0, QApplication::UnicodeUTF8)
        );
        label_3->setText(QApplication::translate("MainWindow", "Input Color Profile:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Output Color Profile:", 0, QApplication::UnicodeUTF8));
        pushButtonStart->setText(QApplication::translate("MainWindow", "Start Conversion Process", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Rendering Intent:", 0, QApplication::UnicodeUTF8));
        comboBox_renderingIntent->clear();
        comboBox_renderingIntent->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Absolute Colormetric", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Perceptual", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Relative Colormetric", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Saturation", 0, QApplication::UnicodeUTF8)
        );
        pushButton_clearDisplay->setText(QApplication::translate("MainWindow", "Clear Display", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
