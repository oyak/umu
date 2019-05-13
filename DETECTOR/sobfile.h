#ifndef SOBFILE_H
#define SOBFILE_H

#include <QString>
#include <QFile>
#include "test.h"

#define cFileVersion 1

extern const QString SOBFileExtension;
extern const char SOBFileSignature[];



// ����� ��������� ��������� SOBSize �������� ������ tPathStepData, ������� ���������� �����

//[    tPathStepData "��������� ������ ��� ������ ������� ��"
// ������ � ������
// 1                                  ���������� ������� QChannels, ��� ������� �������� ������ (����� ���� �����, ���� ��� ������ ��������)
// 1                                  CIDn - ������������� ������, ��� �������� ���� ����� ������
// 1                                  qSignals - ���������� ��������  (��������� ��������)
// qSignals*sizeof(tDaCo_BScanSignal) qSignals �������� tDaCo_BScanSignal
//]
#pragma pack (push,1)
struct tSOBHeader
{
    uint32_t FileSignature;
    uint32_t MD5Sum; // ������ ��� ����� �����, ������� � FileVersion
    uint32_t FileVersion;
    uint32_t Id; // ������������� �������
    uint32_t ObjectOrder;
    uint32_t SOBSize;  // ������ ������� � ����� ��
    int32_t  N0EMSShift;
    uint32_t Size; // ������ � ��
    uint32_t PathStep; // ��� �� � ����� ����� ��
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
