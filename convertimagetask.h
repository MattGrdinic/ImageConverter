#ifndef CONVERTIMAGETASK_H
#define CONVERTIMAGETASK_H

#include <QObject>

#include <QThread>
#include <QStringList>
#include <QStringListIterator>
#include <QDebug>

#include <QImage>
#include <QColor>
#include <QRgb>
#include <QFile>

//#include <jpeglib.h>

//#include "tiffio.h"

#include "lcms2.h"

#include <immintrin.h>
//#include <pmmintrin.h>
//#include <tmmintrin.h>

class ConvertImageTask : public QThread
{
    Q_OBJECT

public:
    ConvertImageTask(QObject *reciever,
                     volatile bool *stopped, const QStringList &sourceFiles,
                     const QString &targetType, const QString &path,
                     const int &optimizationLevel,
                     const int renderingIntent,
                     QByteArray inProfileData, QByteArray outProfileData);

signals:
    void threadUpdate(QString);

public slots:

private :
    void run();

    QObject *reciever;
    volatile bool *stopped;
    QStringList sourceFiles;
    QString targetType;
    QString path;
    int optimizationLevel;
    int renderingIntent;
    QByteArray inProfileData;
    QByteArray outProfileData;

};

#endif // CONVERTIMAGETASK_H
