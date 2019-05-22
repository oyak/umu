#include "sobfmaker.h"
#include <QDebug>
#include <QVector>
#include <QTextCodec>

// конструктор, при использовании класса для создания файлов sob
SOBFMAKER::SOBFMAKER(QString filePathToDestination, QString filePathToSource)
{
    _destinationFilePath = filePathToDestination;
    _sourceFilePath = filePathToSource;
    _pFileParser = new Test(filePathToSource);
}

// конструктор, при использовании класса для чтения файлов sob
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
bool res = false;
tSCANOBJECT_EX object;
tSOBHeader header;
SignalsData *pSignalsData;
unsigned int signalCounter;
QVector<unsigned char> fileBlock;
unsigned char amplitude;
unsigned char delay;
QString fileName;
QString filePath;

    assert(_pFileParser); // нужно вызывать правильный конструктор

//        qDebug() << "offset coord = " << _pFileParser->countCoordUntilFileOffet(81, 0x67c4fe);

    if (_pFileParser->extractObject(lib, objectId, object) )
    {
        header.ObjectOrder = object.ObjectOrder;
        header.Size = object.Size;
        header.Id = objectId;
        header.N0EMSShift = object.N0EMSShift;
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
                   fileBlock.push_back((unsigned char)signalCounter); // кол-во сигналов
                   for(unsigned int ii=0; ii < signalCounter; ++ii)
                   {
                       res = pSignalsData->getSignalParamters(*it, amplitude, delay, ii);
                       assert(res);
                       fileBlock.push_back(delay);

//                       if (delay == 0)
//                       {
//                           qWarning() << "object Id =" << objectId <<  ", step =" << step << ", CID = " << *it << ", signal =" << ii << "delay == 0";
//                       }

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

// создает объект tSCANOBJECT_EX и возвращает на него указатель
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
                            if (_pFile->readByte(&b)) // количество сигналов - ненулевое значение
                            {
                                if (b)
                                {
                                    unsigned int s;
                                    qSignals = (unsigned int)b;
                                    for(s=0; s < qSignals; ++s)
                                    {
                                        if (_pFile->readByte(&b) == false) break;
                                        delay = b;

//                                        if (delay == 0)
//                                        {
//                                            qWarning() << "object Id =" << objectId <<  ", step =" << step << ", CID = " << channel << ", signal =" << s << "delay == 0";
//                                        }

                                        if (_pFile->readByte(&b) == false) break;
                                        amplitude = b;
                                        if (delay)
                                        { // сигналы с нулевыми задержками не включаем в список
                                          // такие сигналы лучше сразу не включать в файл sob
                                            if (pSignalsData->addSignalToList(&signalList, amplitude, delay, s) == false) break;
                                        }
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
            pObjectEx->N0EMSShift = header.N0EMSShift;
            pObjectEx->Size = (eOBJECT_ORDER)header.Size;
        }
            else
            {
                delete pObject;
            }
    }
    _pFile->close();
    delete _pFile;
    return pObjectEx;
}
//
void SOBFMAKER::createSOBFiles(bool restoreAbsentFileOnly)
{
QVector<unsigned int> idsOfObjects;
QVector<unsigned int>::iterator it;
OBJECTLIB lib;
    lib.getAllIds(idsOfObjects);
//
    if (restoreAbsentFileOnly)
    {
        for(it = idsOfObjects.begin(); it != idsOfObjects.end(); )
        {
            QString fileName;
            QString filePath;
            SOBFile::compileFileName(fileName, *it);
            filePath = _destinationFilePath + "/" + fileName;
            if (QFile::exists(filePath))
            {
                it = idsOfObjects.erase(it);
            }
                else ++it;
        }
    }
//
    _cancellFlag = false;
    if (!idsOfObjects.empty())
    {
        for(it = idsOfObjects.begin(); it != idsOfObjects.end(); ++it)
        {
            QString nStr;
            nStr.setNum(*it);
            emit resultMessage("Creating " + nStr + " object");
            if (createFile(*it)) emit resultMessage("OK");
                else emit resultMessage("failed");
            if (_cancellFlag) break;
        }
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

// вызываем для отмены операции пользователем
void SOBFMAKER::onCancell()
{
    _cancellFlag = false;
}
