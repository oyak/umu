#ifndef SOBFMAKER_H
#define SOBFMAKER_H

#include "sobfile.h"
#include "objectlib.h"

class SOBFMAKER
{
public:
    SOBFMAKER(QString filePathToDestination, QString filePathToSource);
    SOBFMAKER(QString filePath);
    ~SOBFMAKER();

    bool createFile(unsigned int objectId, Test::eMovingDir movingDirection);
    tSCANOBJECT_EX *restoreObjectFromFile(unsigned int objectId, Test::eMovingDir movingDirection);


private:
    SOBFile *_pFile;
    OBJECTLIB lib;
    QString _filePath;
    Test *_pFileParser;
};

#endif // SOBFMAKER_H
