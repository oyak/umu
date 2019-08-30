#include "logfile.h"
#include <QDir>
#include <QTime>

LOGFILE::LOGFILE(QString *filePath, QString *fileName)
{
QDir dir(*filePath);
    if (dir.exists())
    {
        _fileName = *filePath + "/" + *fileName;
    }

}

void LOGFILE::startBlock()
{
QString blockHeader;
QTime currentT = QTime::currentTime();
    addString(blockHeader); // выведем пустую строку
    blockHeader = QString::asprintf("-------------------- %d:%d:%d:%d --------------------", currentT.hour(), currentT.minute(), currentT.second(), currentT.msec());
    addString(blockHeader);
}

void LOGFILE::addNote(QString note)
{
    addString(note);
}

void LOGFILE::addString(QString& string)
{
QChar *pData;
    pData = string.data();
    if (_fileName.isEmpty() == false)
    {
        QFile fileHandle(_fileName);
        if(fileHandle.open(QIODevice::ReadWrite))
        {
            char ch;
            fileHandle.seek(fileHandle.size());
            while(!pData->isNull())
            {
                ch = pData->toLatin1();
                fileHandle.write(&ch, 1);
                pData++;
            }
            ch = '\n';
            fileHandle.write(&ch, 1);
            fileHandle.flush();
            fileHandle.close();
        }
    }
}
