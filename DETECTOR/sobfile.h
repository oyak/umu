#ifndef SOBFILE_H
#define SOBFILE_H

#include <QString>
#include <QFile>
#include "test.h"

#define cFileVersion 1

extern const QString SOBFileExtension;
extern const char SOBFileSignature[];



// после заголовка находятся SOBSize структур данных tPathStepData, имеющих переменную длину

//[    tPathStepData "структура данных для одного отсчета ДП"
// размер в байтах
// 1                                  количество каналов QChannels, для которых записаны данные (может быть нулем, если нет вообще сигналов)
// 1                                  CIDn - идентификатор канала, для которого идут далее данные
// 1                                  qSignals - количество сигналов  (ненулевое значение)
// qSignals*sizeof(tDaCo_BScanSignal) qSignals структур tDaCo_BScanSignal
//]
#pragma pack (push,1)
struct tSOBHeader
{
    uint32_t FileSignature;
    uint32_t MD5Sum; // входят все байты файла, начиная с FileVersion
    uint32_t FileVersion;
    uint32_t Id; // идентификатор объекта
    uint32_t ObjectOrder;
    uint32_t SOBSize;  // размер объекта в шагах ДП
    int32_t  N0EMSShift;
    uint32_t Size; // размер в мм
    uint32_t PathStep; // шаг ДП в сотых долях мм
    tSOBHeader():
        MD5Sum(0),
        N0EMSShift(0),
        FileVersion(cFileVersion)
    {

        memcpy((char*)&FileSignature, SOBFileSignature, sizeof(int32_t));
    }
};

#pragma pack (pop)

class SOBFile
{

public:
    static void compileFileName(QString& fileName, unsigned int objectId);

    SOBFile(QString& filePathAndName, QIODevice::OpenMode modeFlags);
    ~SOBFile();

    bool readHeader(tSOBHeader *pHeader, bool DontCloseFile); //
    bool writeHeader(tSOBHeader *pHeader, bool DontCloseFile);
    bool writeBlock(QVector<unsigned char>& data);
    bool readByte(unsigned char *pByte);

    bool isFileCorrect();
    void close();

private:
    QFile *_fileHandle;
    QIODevice::OpenMode _openMode;
    bool _opened;
//
    unsigned int MaxSignalAtBScanBlock;

    bool open();
};


#endif // SOBFILE_H
