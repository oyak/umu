#ifndef SOBFMAKER_H
#define SOBFMAKER_H

#include <QObject>
#include <QString>
#include "sobfile.h"
#include "objectlib.h"

class SOBFMAKER: public QObject
{
    Q_OBJECT
public:
    SOBFMAKER(QString filePathToDestination, QString filePathToSource);
    SOBFMAKER(QString filePath);
    ~SOBFMAKER();

    bool createFile(unsigned int objectId, Test::eMovingDir movingDirection);
    tSCANOBJECT_EX *restoreObjectFromFile(unsigned int objectId, Test::eMovingDir movingDirection);
    void createSOBFiles(bool restoreAbsentFileOnly);
    bool testDetectorFilesPresence();

signals: void resultMessage(QString);
public slots: void onCancell();


private:
    SOBFile *_pFile;
    OBJECTLIB lib;
    QString _sourceFilePath;
    QString _destinationFilePath;
    Test *_pFileParser;
    bool _cancellFlag;
};

#endif // SOBFMAKER_H
