#include "imagemanager.h"

ImageManager::ImageManager() { }

ImageManager::~ImageManager() { };

void ImageManager::processImagesThreaded(const QString path, int optimizationLevel,
                                         const int renderingIntent,
                                         const int inProfile, const int outProfile)
{

    QString processedPath = path + "/_processed";

    // create processed folder if needed
    if(!QDir(processedPath + "_processed").exists()){

        QDir imgPath(processedPath);
        if(!imgPath.mkpath(processedPath)){
            qDebug() << "Cannot create image processed folder.";
        }
    }

    QStringList filters, sourceFiles;
    filters << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG" << "*.tif" << "*.tiff";

    // populate valid files
    QDirIterator i(path, filters, QDir::Files);
    while(i.hasNext()){
        i.next();
        QString fileName = i.fileName();
        if(fileName == "."
                        || fileName == ""
                        || fileName == ".."
                        || fileName == ".DS_Store"){
            continue;
        }
        sourceFiles << i.fileName();
    }

    threadCount = 0;

    volatile bool stopped = true;

    const QVector<int> sizes = chunkSizes(sourceFiles.count(), QThread::idealThreadCount());

    int offset = 0;

    QByteArray inProfileData; QByteArray outProfileData;

    this->loadInputColorPofile(inProfile, inProfileData);
    this->loadOutColorPofile(outProfile, outProfileData);

    foreach(const int chunkSize, sizes){
        if(chunkSize != 0){
            ConvertImageTask *convertImageTask =
                    new ConvertImageTask(this, &stopped,
                      sourceFiles.mid(offset, chunkSize),
                      QString("JPEG"),
                      path,
                      optimizationLevel,
                      renderingIntent,
                      inProfileData,
                      outProfileData);


            convertImageTask->start();

            connect(convertImageTask, SIGNAL(finished()),
                    this, SLOT(threadFinished()));

            connect(convertImageTask, SIGNAL(threadUpdate(QString)),
                    this, SLOT(threadUpdate(QString)));

            offset += chunkSize;
            threadCount++;
        }
    }

}

void ImageManager::threadUpdate(QString message)
{
    //qDebug() << message;
    emit threadMessage(message);
}

void ImageManager::threadFinished()
{
    --threadCount;

    if(threadCount == 0){
        emit threadsFinsihed();
        //qDebug() << "Threads Finsihed";
    }
}

QVector<int> ImageManager::chunkSizes(int files, int chunksize)
{
    QVector<int> chunks;

    if((files % chunksize) == 0){
        for(int i = 0; i < chunksize; i++){
            chunks.append((files / chunksize));
        }
    } else {
        for(int i = 0; i < chunksize; i++){
            if((files % (chunksize - i)) == 0){
                int appendVal = files / (chunksize - i);
                for(int j = i; j < chunksize; j++, i++){
                    chunks.append(appendVal);
                }
            } else {
                chunks.append(std::ceil((float)files / (chunksize - i)));
                files -= std::ceil((float)files / (chunksize - i));
            }

        }
    }

    return chunks;
}

void ImageManager::loadInputColorPofile(const int profile, QByteArray &profileData)
{
    QString inProfileValue = "";

    switch(profile){
    case 0 :
        inProfileValue = ":/resources/Adobe ICC Profiles/CMYK/USWebUncoated.icc";
        break;
    case 1 :
        inProfileValue = ":/resources/Adobe ICC Profiles/CMYK/USWebCoatedSWOP.icc";
        break;
    }

    // get memory reference to in profile file data
    QFile fIn(inProfileValue);
    if(!fIn.open(QIODevice::ReadOnly))
        qDebug() << "Cannot load input profile";

    QByteArray byte_array(fIn.readAll());

    // reference pass

    profileData = byte_array;
}


void ImageManager::loadOutColorPofile(const int profile, QByteArray &profileData)
{

    QString outProfileValue = "";

    switch(profile){
    case 0 :
        outProfileValue = ":/resources/Adobe ICC Profiles/RGB/AdobeRGB1998.icc";
        break;
    case 1 :
        outProfileValue = ":/resources/Adobe ICC Profiles/RGB/AppleRGB.icc";
        break;
    case 2 :
        outProfileValue = ":/resources/Adobe ICC Profiles/RGB/ColorMatchRGB.icc";
        break;
    }

    // get memory reference to out profile file data
    QFile fOut(outProfileValue);
    if(!fOut.open(QIODevice::ReadOnly))
       qDebug() << "Canot load output profile";

    QByteArray outBuffer;
    outBuffer = fOut.readAll();

    // reference pass

    profileData = outBuffer;

}
