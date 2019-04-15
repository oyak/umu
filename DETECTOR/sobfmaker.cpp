#include "sobfmaker.h"
#include <QDebug>
#include <QVector>
#include <QTextCodec>

// �����������, ��� ������������� ������ ��� �������� ������ sob
SOBFMAKER::SOBFMAKER(QString filePathToDestination, QString filePathToSource)
{
    _destinationFilePath = filePathToDestination;
    _sourceFilePath = filePathToSource;
    _pFileParser = new Test(filePathToSource);
}

// �����������, ��� ������������� ������ ��� ������ ������ sob
SOBFMAKER::SOBFMAKER(QString filePath)
{
    _destinationFilePath = filePath;
    _pFileParser = nullptr;
}

SOBFMAKER::~SOBFMAKER()
{
    delete _pFileParser;
}

bool SOBFMAKER::createFile(unsigned int objectId)
{
bool res;
tSCANOBJECT_EX object;
tSOBHeader header;
SignalsData *pSignalsData;
unsigned int signalCounter;
QVector<unsigned char> fileBlock;
unsigned char amplitude;
unsigned char delay;
QString fileName;
QString filePath;

    assert(_pFileParser); // ����� �������� ���������� �����������

//        qDebug() << "offset coord = " << _pFileParser->countCoordUntilFileOffet(81, 0x67c4fe);

    if (_pFileParser->extractObject(lib, objectId, object) )
    {
        header.ObjectOrder = object.ObjectOrder;
        header.Size = object.Size;
        header.Id = objectId;
        header.SOBSize = object.pScanObject->size();
        header.PathStep = object.pScanObject->getPathStep();
    }
        else return false;
//
    SOBFile::compileFileName(fileName, objectId);
    filePath = _destinationFilePath + "/" + fileName;
    _pFile = new SOBFile(filePath, QIODevice::ReadWrite);
    if (_pFile->writeHeader(&header, true))
    {
        for(unsigned int step = 0; step < header.SOBSize; ++step)
        {
        QVector<CID> channels;
        QVector<CID>::iterator it;
           fileBlock.clear();
           pSignalsData = object.pScanObject->data(step);
           pSignalsData->getAllChannels(channels);
           fileBlock.push_back(static_cast<unsigned char>(channels.size()));
           if (channels.size())
           {
               for(it = channels.begin(); it != channels.end(); ++it)
               {
                   fileBlock.push_back(static_cast<unsigned char>(*it)); // CID
                   pSignalsData->getNumOfSignals(*it, &signalCounter);
                   assert(signalCounter != 0);
                   fileBlock.push_back((unsigned char)signalCounter); // ���-�� ��������
                   for(unsigned int ii=0; ii < signalCounter; ++ii)
                   {
                       res = pSignalsData->getSignalParamters(*it, amplitude, delay, ii);
                       assert(res);
                       fileBlock.push_back(delay);
                       fileBlock.push_back(amplitude);
                   }
               }
               channels.clear();
           }
           res = _pFile->writeBlock(fileBlock);
           if (!res) break;
        }
        fileBlock.clear();
        _pFile->close();
        delete _pFile;
    }
    return res;
}

// ������� ������ tSCANOBJECT_EX � ���������� �� ���� ���������
tSCANOBJECT_EX *SOBFMAKER::restoreObjectFromFile(unsigned int objectId)
{
tSCANOBJECT_EX *pObjectEx = nullptr;
SCANOBJECT *pObject;
tSOBHeader header;
SignalsData *pSignalsData;
QString fileName;
QString filePath;
unsigned char qChannels;
CID channel;

    pObject = new SCANOBJECT;
    SOBFile::compileFileName(fileName, objectId);
    filePath = _destinationFilePath + "/" + fileName;
    _pFile = new SOBFile(filePath, QIODevice::ReadOnly);
    if (_pFile->readHeader(&header, true))
    {
        unsigned int step;
        for(step = 0; step < header.SOBSize; ++step)
        {
            pSignalsData = new SignalsData;
            if (_pFile->readByte(&qChannels))
            {
                unsigned int channelCnt;
                if (qChannels)
                {
                    for(channelCnt=0; channelCnt < qChannels; ++channelCnt)
                    {
                        unsigned char b;
                        tDaCo_BScanSignalList signalList;
                        unsigned char amplitude;
                        unsigned char delay;
                        unsigned int qSignals;
                        if (_pFile->readByte(&b))
                        {
                            channel = (CID)b;
                            if (_pFile->readByte(&b)) // ���������� �������� - ��������� ��������
                            {
                                if (b)
                                {
                                    unsigned int s;
                                    qSignals = (unsigned int)b;
                                    for(s=0; s < qSignals; ++s)
                                    {
                                        if (_pFile->readByte(&b) == false) break;
                                        delay = b;
                                        if (_pFile->readByte(&b) == false) break;
                                        amplitude = b;
                                        if (pSignalsData->addSignalToList(&signalList, amplitude, delay, s) == false) break;
                                    }
                                    if (s == qSignals)
                                    {
                                        if (pSignalsData->addSignals(channel, qSignals, &signalList) == false) break;
                                    }
                                        else break;
                                }
                                    else break;
                            }
                                else break;
                        }
                            else break;
                    }
                    if (channelCnt < qChannels) break;
                 }
                 pObject->add(pSignalsData);
                 delete pSignalsData;
              }
        }
        if (step == header.SOBSize)
        {
            pObject->setPathStep(header.PathStep);
            pObjectEx = new tSCANOBJECT_EX(pObject);
            pObjectEx->Id = header.Id;
            pObjectEx->ObjectOrder = (eOBJECT_ORDER)header.ObjectOrder;
            pObjectEx->Size = (eOBJECT_ORDER)header.Size;
        }
            else
            {
                delete pObject;
            }
    }
    return pObjectEx;
}
//
void SOBFMAKER::createSOBFiles()
{
QVector<unsigned int> idsOfObjects;
QVector<unsigned int>::iterator it;
OBJECTLIB lib;
    lib.getAllIds(idsOfObjects);
    _cancellFlag = false;
    for(it = idsOfObjects.begin(); it != idsOfObjects.end(); ++it)
    {
        QString nStr;
        nStr.setNum(*it);
        emit resultMessage("Creating " + nStr + "object");
        if (createFile(*it)) emit resultMessage("OK");
            else emit resultMessage("failed");
        if (_cancellFlag) break;
        sleep(10);
    }
}
//
bool SOBFMAKER::testDetectorFilesPresence()
{
bool res = true;
OBJECTLIB lib;
QVector<QString> FileNamesArray;
QVector<QString>::iterator it;
//QTextCodec *pCodec;

//    pCodec = QTextCodec::codecForName("Windows-1251");

//    pCodec = QTextCodec::codecForLocale();
    lib.getAllSorceFiles(FileNamesArray);
    for (it = FileNamesArray.begin(); it != FileNamesArray.end(); ++it)
    {
        if (!QFile::exists(_sourceFilePath + "/" + it->toUtf8()))
        {
            emit resultMessage("File " + *it + "not found");
            res = false;
//            break;
        }
    }
    return res;
}

// �������� ��� ������ �������� �������������
void SOBFMAKER::onCancell()
{
    _cancellFlag = false;
}

\
