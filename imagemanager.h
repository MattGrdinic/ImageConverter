#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QDirIterator>
#include <QMap>

#include <cmath>

#include "convertimagetask.h"

class ImageManager : public QObject
{
    Q_OBJECT

public:
    ImageManager();
    ~ImageManager();

    QVector<int> chunksSizes(int, int);
    int threadCount;

    void processImagesThreaded(const QString, const int, const int renderingIntent,
                               const int inProfile, const int outProfile);

    QVector<int> chunkSizes(int files, int chunksize);

    void loadInputColorPofile(const int, QByteArray &profileData);
    void loadOutColorPofile(const int, QByteArray &profileData);

signals :
    void statusMessage(int, QString);
    void threadMessage(QString);
    void threadsFinsihed();

public slots :
    void threadUpdate(QString);
    void threadFinished();


};

#endif // IMAGEMANAGER_H
