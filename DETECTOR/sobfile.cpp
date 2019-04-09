#include <QDir>
#include "sobfile.h"
#include "test.h"

const QString SOBFileExtension("sob");
const char SOBFileSignature[] = {"sobv"};

void SOBFile::compileFileName(QString& fileName, unsigned char objectId, Test::eMovingDir movingDirection)
{
    assert(movingDirection != Test::DirNotDefined);
    fileName.clear();
    fileName = "obj";
    if (movingDirection != Test::DirUpWard) fileName += "d"; // признак движения по ходу увеличения координаты
        else fileName += "r";
    fileName += QString::number(objectId) + "." + SOBFileExtension;
}


SOBFile::SOBFile(QString &filePathAndName, QIODevice::OpenMode modeFlags)
{
bool res = true;
//    QDir dir;
//    QString path = dir.filePath(filePathAndName);
//    res = dir.cd(path);
    _fileHandle = nullptr;
    _opened = false;
    _openMode = modeFlags;
    if (_openMode == QIODevice::ReadWrite)
    {

//       if (!res) dir.mkpath(path);
        res = true;

    }
        else
        {
            if (res)
            {
                res = QFile::exists(filePathAndName);
            }
        }

    if (res)
    {
        _fileHandle = new QFile(filePathAndName);
    }
}

SOBFile::~SOBFile()
{
    close();
    delete _fileHandle;
}

//char *QByteArray::data()
// возвращает true, если заголовок корректный
bool SOBFile::readHeader(tSOBHeader *pHeader, bool DontCloseFile)
{
bool res = false;
    if (_fileHandle == nullptr) return false;
    open();
    if (_opened)
    {
        _fileHandle->seek(0);
        if (_fileHandle->read((char*)pHeader, sizeof(tSOBHeader)) == sizeof(tSOBHeader))
        {
            res = ((pHeader->FileVersion == cFileVersion) &&
                  (strncmp((const char*)(&pHeader->FileSignature), SOBFileSignature, sizeof(int32_t)) == 0));
        }
        if(!DontCloseFile) close();
    }
    return res;
}

bool SOBFile::writeHeader(tSOBHeader *pHeader, bool DontCloseFile)
{
bool res;
    assert(_fileHandle != nullptr);
    close();
    res = open();
    if (res)
    {
        res = (_fileHandle->write((char*)pHeader, sizeof(tSOBHeader)) != 0);
        if (res) _fileHandle->flush();
        if(!DontCloseFile) close();
    }
    return res;
}

bool SOBFile::isFileCorrect()
{
    return true;
}

void SOBFile::close()
{
    if (_opened)
    {
        _fileHandle->close();
        _opened = false;
    }
}

bool SOBFile::open()
{
    if (_opened == false)
    {
        _opened = _fileHandle->open(_openMode);
    }
    return _opened;
}

bool SOBFile::writeBlock(QVector<unsigned char>& data)
{
bool res;
    assert(_fileHandle != nullptr);
    res = (_fileHandle->write((const char*)data.data(), data.size()) != 0);
    if (res) _fileHandle->flush();
    return res;
}

bool SOBFile::readByte(unsigned char *pByte)
{
bool res;
//qint64 pos;
    assert(_fileHandle != nullptr);
//    pos = _fileHandle->pos();
    res = (_fileHandle->read((char*)pByte, 1) != 0);
    return res;
}
