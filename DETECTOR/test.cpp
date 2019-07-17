#include <assert.h>
#include <QDebug>
#include <QDir>
#include <QtEndian>
#include "stdio.h"
#include "test.h"
#include "signalsdata.h"
#include "ChannelsIds.h"

#pragma pack(push, 1)

typedef struct
{
  unsigned char Side ; // 1 - Right
  unsigned char ChannelIdxInFile;
  unsigned char Value;
} tEventByteData;
//
typedef struct
{
  unsigned char Side ; // 1 - Right
  unsigned char ChannelIdxInFile;
  unsigned short Value;
} tEventWordData;

#pragma pack(pop)



Test::~Test()
{
    closeFile();
}

bool Test::extractObject(OBJECTLIB& lib, unsigned int objectId, tSCANOBJECT_EX& object)
{
tObjectSourceDescr descriptor;
bool res;
sCoordPostMRF startPost;
int startKM;
int startPk;
int startM;
int startmM;

    res = lib.getRecordData(objectId, descriptor);

    if (res)
    {
        res = openFile(descriptor.SourceFileName);
    }
    if (res)
    {
        res = readHeader();
    }
        else return res;
//
    if (res)
    {
        startPost.Km[1] = startKM = descriptor.StartKm;
        startPost.Pk[1] = startPk = descriptor.StartPk;
        startM = descriptor.StartM;
        startmM = descriptor.StartmM;
        if (descriptor.Order == ExpandedOverPlacing)
        {
            assert(abs(descriptor.N0EMSShift) < 1000);
            if (descriptor.N0EMSShift < 0)
            { // начальную точку объекта сдвигаем влево
                if (_Header.MoveDir == -1) { // координата должна увеличиться
                tMRFCrd post;
                int pkLen; // длина текущего участка в мм
                int mm;
                int m;
                    res = getPKLen(&startPost, pkLen, false);
                    assert(res); // следующий пикетный столб не найден
                    assert(pkLen);
                    m = pkLen / 1000;
                    assert(m);
                    mm = pkLen % 1000;
                    startmM -= descriptor.N0EMSShift;
                    if ((startM == m-1) &&  (startmM > mm)) {
                        post.Km = startKM;
                        post.Pk = startPk;
                        post = GetPrevMRFPostCrd(post, _Header.MoveDir);
                        startKM = post.Km;
                        startPk = post.Pk;
                        startM = 0;
                        startmM -= mm;
                    }
                        else {// не отошли за предыдущий пикетный столб
                            if (startmM >= 1000)
                            {
                                startmM -= 1000;
                                startM++;
                            }
                        }
                }
                    else {//
                    startmM += descriptor.N0EMSShift;
                    if (startmM < 0)
                    {
                        if (startM == 0)
                        {
                            tMRFCrd post;
                            int pkLen; // длина прерыдущего участка в мм
                            int mm;
                            int m;
                            res = getPKLen(&startPost, pkLen, true);
                            if (res)
                            {
                                assert(pkLen);
                                m = pkLen / 1000;
                                mm = pkLen % 1000;
                                if (m == 0)
                                { // и такое безобразие, наыерное, может быть
                                    assert(mm >= abs(startmM));
                                    startmM += mm;
                                    startM = m;
                                }
                                else
                                {
                                    if (mm >= abs(startmM))
                                    {
                                        startmM += mm;
                                        startM = m;
                                    }
                                    else
                                    {
                                        startmM += 1000 + mm;
                                        startM = m - 1;
                                    }
                                }
                                post.Km = startKM;
                                post.Pk = startPk;
                                post = GetPrevMRFPostCrd(post, _Header.MoveDir);
                                startKM = post.Km;
                                startPk = post.Pk;
                            }
                                else
                                {
                                    assert(0); // предыдущий пикетный столб не найден
                                }
                        }
                        else
                        {//если не отошли за предыдущий пикетный столб
                            startmM += 1000;
                            startM --;
                        }
                    }
                }
            }
        }
    }
//
    if (res)
    {
        bool condition1;
        bool condition2; //
        if (_Header.MoveDir == -1)
        {
            condition1 = (_Header.StartKM >= startKM);
            condition2 = (_Header.StartKM > startKM) || (_Header.StartKM == startKM) && (_Header.StartPk > startPk);
        }
            else
            {// файл в сторону увеличения путейской координаты
                condition1 = (_Header.StartKM <= startKM);
                condition2 = (_Header.StartKM < startKM) || (_Header.StartKM == startKM) && (_Header.StartPk < startPk);
            }
        if (condition1)
        {
            startPost.Km[1] = startKM;
            startPost.Pk[1] = startPk;
            unsigned int systemCoord;
            if (condition2)
            {
              int currentCoord = _fullCoordinate;
              if (_Header.MoveDir == -1) {
              int pkLen; // длина текущего участка в мм
                  res = getPKLen(&startPost, pkLen, false);
                  assert(res); // следующий пикетный столб не найден
                  assert(pkLen);
                  res = findAndParseStolbID(startPost, &currentCoord, _Header.MoveDir);
                  if (res)
                  {
                      _fullCoordinate = currentCoord;
                      qDebug() << "Disared Stolb found - currentCoord = " << _fullCoordinate << "fileOffset = " << _fileOffset;
                      systemCoord = _fullCoordinate + convertMMToSystemCoord(pkLen - startmM - 1000 * startM);
                  }
              }
              else {
                  res = findAndParseStolbID(startPost, &currentCoord, _Header.MoveDir);
                  if (res)
                  {
                      _fullCoordinate = currentCoord;
                      qDebug() << "Disared Stolb found - currentCoord = " << _fullCoordinate << "fileOffset = " << _fileOffset;
                      systemCoord = _fullCoordinate + convertMMToSystemCoord(startM * 1000 + startmM);
                  }
              }
            }
                else
                {// начинаем на начальном пикете файла
                    if (_Header.MoveDir == -1) {
                        if (_Header.StartMetre >= startM)
                        {
                        int pkLen; // длина текущего участка в мм
                            res = getPKLen(&startPost, pkLen, false);
                            assert(res); // следующий пикетный столб не найден
                            assert(pkLen);
                            systemCoord = _fullCoordinate + convertMMToSystemCoord(pkLen - startmM - (_Header.StartMetre - startM) * 1000);
                        }
                            else res = false;
                    }
                    else {
                        if (_Header.StartMetre <= startM)
                        {
                            systemCoord = _fullCoordinate + convertMMToSystemCoord((startM - _Header.StartMetre) * 1000 + startmM);
                        }
                        else res = false;
                    }
                }
            if (res)
            {
               qWarning() << QString::asprintf("object = %d, start system coord in file = 0x%x", objectId, systemCoord);
               unsigned int objSize = descriptor.LengthMM + abs(descriptor.N0EMSShift) - descriptor.LngCutting - 2;
               res = extractScanObject(systemCoord, descriptor.Side, objSize, object);
               object.ObjectOrder = descriptor.Order;
               object.Size = objSize;
               object.N0EMSShift = descriptor.N0EMSShift;
            }
         }
             else res = false;
    }
    closeFile();
    return res;
}

bool Test::openFile(QString& fileName)
{
bool res = false;
    if (_pFile == NULL)
    {
//        _pFile = new QFile("av31-1.a31"); //180609-132919.a31

//        _pFile = new QFile("180609-132919.a31"); //180609-132919.a31

//        _pFile = new QFile("180927-124849.a31"); //180609-132919.a31

        if (!_pathToFiles.isEmpty())
        {
         QDir dir;
            dir = QDir::currentPath();
           _pFile = new QFile(dir.path() + "/" + _pathToFiles + "/" + fileName);
        }
            else
            {
                _pFile = new QFile(fileName);
            }
        assert(_pFile->exists());

        if (_pFile->open(QIODevice::ReadOnly/*Write*/ ) == true)
        {
            res = true;
        }
            else
            {
                delete _pFile;
                _pFile = NULL;
            }
    }
    return res;
}
void Test::closeFile()
{
    if (_pFile)
    {
        _pFile->close();
        delete _pFile;
        _pFile = nullptr;
    }
}

bool Test::readHeader()
{
    _fullCoordinate = 0;
    _fileOffset = 0;
    return readHeader(true, &_Header);
}

bool Test::readHeader(bool toResetFilePos, sFileHeader_v5 *pHeader)
{
qint64 rBytes;
unsigned int error;
bool res = true;

    assert(_pFile != NULL);
    if(toResetFilePos)
    {
        _fullCoordinate = 0;
        _pFile->seek(0);
    }
//    _fileHeader = _pFile->read(cHeaderLen);
    rBytes = _pFile->read(reinterpret_cast<char*>(pHeader), sizeof(_Header));
    if (rBytes == 0)
    {
        error = _pFile->error();
        res = false;
    }
    return res;
}

/*
// false, если ошибка разбора
bool Test::skipFile()
{
sFileHeader_v5 tempHeader;
unsigned char Id;
bool res;

    assert(_pFile != NULL);
    readHeader(false, &tempHeader);
    if (tempHeader.HeaderVer != 5)
    {
    //    assert(0);
    }
    do
    {
        res = readAndParseEventID(Id);

    }
    while ((res != false) && (Id != EID_EndFile) ) ;
    if (res == false)
    {
        qDebug() << "Ошибка разбора. Позиция" << _fileOffset;
    }
    return res;
}
*/
//
// //
// startCoord - начальная системная координата объекта
// side - сторона, к которой он относится
// len - длина объекта в мм
bool Test::extractScanObject(unsigned int startCoord, eUMUSide side, unsigned int len, tSCANOBJECT_EX& object)
{
SignalsData signalsData;
bool res;
unsigned int offset = 0;
    len = convertMMToSystemCoord(len); // переведем в шаги ДП
    if (!object.pScanObject->isEmpty()) object.pScanObject->clear();
    object.pScanObject->setPathStep(_Header.ScanStep);
    do
    {
        res = extractSignalsByCoord(startCoord + offset, side, signalsData);
        if (res)
        {
            object.pScanObject->add(&signalsData);
            offset++;
        }
    } while((res) && (offset < len));
    if (offset < len)
    {
        object.pScanObject->clear();
        return false;
    }
    return true;
}

CID Test::convertToCID(CID chIdx, eMovingDir movingDirection)
{
    if (movingDirection != DirDownWard) return chIdx;
        else
        {
            switch(chIdx)
            {
                case N0EMS:
                case N0EMS2:
                    return chIdx;
                case F58ELU:
                    return B58ELW;
                case F58ELW:
                    return B58ELU;
                case B58ELU:
                    return F58ELW;
                case B58ELW:
                    return F58ELU;
                case F70E:
                    return B70E;
                case B70E:
                    return F70E;
                case F42E:
                    return B42E;
                case B42E:
                    return F42E;
                default: assert(0);
            }
    }
}
// определяет длину участка в мм от пикетного столба *postCoordPtr
// до следующего/предыдущего столба(начала файла) в зависимости inverseDirection = false/true
// возвращает true, если пикетный столб найден
// возвращает указатель в файле на позицию, соответствующую после чтения заголовка
bool Test::getPKLen(sCoordPostMRF *postCoordPtr, int& len, bool inverseDirection)
{
bool res;
sCoordPostMRF firstPostCoord;
sCoordPostMRF postCoord2;
int startCoord;
int nextCoord;
    readHeader();
    res = readNextStolb(&firstPostCoord, &startCoord);
    readHeader();
    res = findAndParseStolbID(*postCoordPtr, &startCoord, _Header.MoveDir);
    if (res)
    {
       len = 0;
       if (inverseDirection == false)
       { // направление поиска совпадает с направлением в файле
           res = readNextStolb(&postCoord2, &nextCoord);
           if (res)
           {
               len = convertSystemCoordToMM(nextCoord - startCoord);
           }
       }
           else
           {
               if ((firstPostCoord.Km[1] == postCoordPtr->Km[1]) && (firstPostCoord.Pk[1] == postCoordPtr->Pk[1]))
               { // заданный столб - первый в файле
                   len = convertSystemCoordToMM(startCoord);
               }
                   else
                   {
                   tMRFCrd post;
                       post.Km = postCoordPtr->Km[1];
                       post.Pk = postCoordPtr->Pk[1];
                       post = GetPrevMRFPostCrd(post, _Header.MoveDir);
                       postCoord2.Km[1] = post.Km;
                       postCoord2.Pk[1] = post.Pk;
                       readHeader();
                       res = findAndParseStolbID(postCoord2, &nextCoord, _Header.MoveDir);
                       if (res)
                       {
                           len = convertSystemCoordToMM(startCoord - nextCoord);
                       }
                   }
           }
    }
    readHeader();
    return res;
}

// извлекает сигналы, относящиеся к заданной системной координате,
// для заданной стороны
// возвращает true, если координата найдена
// если заданная координата coord меньше текущей в файле, поиск прекращается
bool Test::extractSignalsByCoord(unsigned int coord, eUMUSide side, SignalsData& signalsData)
{
unsigned int currentCoord = 0;
bool fShort;
bool res;
unsigned char id;
tDaCo_BScanSignals BSSignals; // здесь номер канала в терминах индекса канала в файле

//    qDebug() << "extractSignalsByCoord: coord = " << coord;
    do
    {
        res = readNextCoord(currentCoord, fShort);
        if (fShort)
        {
            currentCoord |= _fullCoordinate & ~0xFF;
        }
            else
            {
                _fullCoordinate = currentCoord;
            }

//    qDebug() << "res = " << res << "currenCoord = " << currentCoord;

    } while ((res) && !(currentCoord >= coord) );
//
    signalsData.clear();
    if (res)
    {// нашли координату
        do
        {
            qDebug() << "currenCoord = " << currentCoord << "fileOffset = " << _fileOffset;

            id = 0xFF;
            res = readAndParseEventID(id, NULL, true);
            if (res == false) break;
            if ( ((id & 0x80) == 0) && (((id & 0x40) >> 6) == getSideByte(side)) )
            {
                res = readAndParseEventID(id, &BSSignals, false);
                if (res == false)
                {
                    signalsData.clear();
                }
                   else
                   {
                       signalsData.addSignals(_Header.ChIdxtoCID[BSSignals.Channel], BSSignals.Count, &BSSignals.Signals);
                   }
            }
                else
                {
                    if ((id != EID_SysCrd_NS) && (id != EID_SysCrd_NF))
                    {
                        id = 0xFF;
                        res = readAndParseEventID(id, NULL, false);
                    }
                        else break; // нашли следующую координату
                }
        } while(true);
    }
    return res;
}
// ищет с текущей позиции в файле идентификатор координаты
// возвращает:
// true - если координата найдена
// по ссылке в coord записывается координата
// short == true - координата короткая
bool Test::readNextCoord(unsigned int &coord, bool& fShort)
{
unsigned char id;
bool res;
unsigned int tempCoord;
    do
    {
        id = 0xFF;
        res = readAndParseEventID(id, NULL, true);
        if (res == true)
        {
            fShort = (id == EID_SysCrd_NS);
            if ((id != EID_SysCrd_NS) && (id != EID_SysCrd_NF))
            {
                id = 0xFF;
                res = readAndParseEventID(id, NULL, false);
                if (res == false) break;
            }
                else
                {
                    fShort = (id == EID_SysCrd_NS);
                    res = readAndParseEventID(id, static_cast<void*>(&tempCoord), false);
                    if (fShort)
                    {
                        coord &= 0xffffff00;
                        coord += tempCoord & 0xFF;
                    }
                        else
                        {
                            coord = tempCoord;
                        }
                    break;
                }

        }
            else
            {
                break;
            }

    } while(true);

    return res;
}
// находит идентификатор заданного столба, устанавливает
// текущую позицию в файле на следующий идентификатор и
// возвращает true
bool Test::findAndParseStolbID(sCoordPostMRF coord, int *systemCoordPtr, int movingDir)
{
bool res;
bool found = false;
sCoordPostMRF currentCoord;
     do
     {
         res = readNextStolb(&currentCoord, systemCoordPtr);
         if (res)
         {
             found = (currentCoord.Km[1] == coord.Km[1]) && (currentCoord.Pk[1] == coord.Pk[1]);
             if (!found)
             {
                 if (movingDir == -1)
                 {
                     if ((currentCoord.Km[1] == coord.Km[1]) && (currentCoord.Pk[1] < coord.Pk[1]) ||
                         (currentCoord.Km[1] < coord.Km[1])) res = false;
                 }
                     else
                     {
                         if ((currentCoord.Km[1] == coord.Km[1]) && (currentCoord.Pk[1] > coord.Pk[1]) ||
                             (currentCoord.Km[1] > coord.Km[1])) res = false;
                     }
             }
         }
     } while(res && !found);
     return res;
}
// ищет с текущей позиции в файле идентификатор EID_Stolb
// возвращает:
// true - если найден
// *postCoordPtr записывается координата
bool Test::readNextStolb(sCoordPostMRF *postCoordPtr, int *systemCoordPtr)
{
unsigned char id;
bool res;
unsigned int tempCoord;
    assert(postCoordPtr);
    do
    {
        id = 0xFF;
        res = readAndParseEventID(id, NULL, true);
        if (res == true)
        {
            if (id != EID_Stolb)
            {
                if ((id != EID_SysCrd_NS) && (id != EID_SysCrd_NF) || (systemCoordPtr == 0))
                {
                    id = 0xFF;
                    res = readAndParseEventID(id, NULL, false);
                    if (res == false) break;
                }
                    else
                    {
                        bool fShort = (id == EID_SysCrd_NS);
                        res = readAndParseEventID(id, static_cast<void*>(&tempCoord), false);
                        if (res == false) break;
                        if (fShort)
                        {
                            *systemCoordPtr &= 0xffffff00;
                            *systemCoordPtr += tempCoord & 0xFF;
                        }
                            else
                            {
                                *systemCoordPtr = tempCoord;
                            }
                    }
            }
                else
                {
                    res = readAndParseEventID(id, static_cast<void*>(postCoordPtr), false);
                    break;
                }
        }
            else
            {
                break;
            }

    } while(true);

    return res;
}

// вход - Id - идентификатор, для которого определен буфер pParsedData
// Id == 0:  pParsedData - буфер для сигналов
// Id == 0xFF: то же самое, что и pParsedData == 0
// если установлен readIDOnly - читаем только ID и возвращаем указатель на исходную позицию
// возвращает:
// - false. если ошибка разбора
// - Id - прочитанный EventID
// - pParsedData используется для EID_SysCrd_NS, EID_SysCrd_NF и сигналов
bool Test::readAndParseEventID(unsigned char& Id, void *pParsedData, bool readIDOnly)
{
bool res = true;
unsigned char ID[4];

    assert(_pFile != NULL);

//    qDebug() << "readAndParseEventID: start file position = " << _pFile->pos();

    if (_pFile->read(reinterpret_cast<char*>(ID), 1) < 1) return false;
    if (readIDOnly)
    {
        Id = ID[0];
        _pFile->seek(_pFile->pos() - 1);
        return true;
    }
//
    if (Id == 0xFF) pParsedData = 0;


    if (ID[0] & 0x80)
    {
    switch(ID[0])
    {
        case EID_AScanFrame:
        case EID_TestRecordFile:        // Файл записи контрольного тупика
        case EID_RailHeadScaner:
        {
            const QByteArray& data = _pFile->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            _pFile->seek(_pFile->pos() + dataSize);
            break;
        }
        case EID_AutomaticSearchRes:
        {
            unsigned char Side = '\0';
            _pFile->getChar(reinterpret_cast<char*>(&Side));
            unsigned char Channel = '\0';
            _pFile->getChar(reinterpret_cast<char*>(&Channel));
            const QByteArray& data = _pFile->read(4);
            int CentrCoord = qFromLittleEndian<int>(reinterpret_cast<const unsigned char*>(data.data()));
            unsigned char CoordWidth = '\0';
            _pFile->getChar(reinterpret_cast<char*>(&CoordWidth));
            unsigned char StDelay = '\0';
            _pFile->getChar(reinterpret_cast<char*>(&StDelay));
            unsigned char EdDelay = '\0';
            _pFile->getChar(reinterpret_cast<char*>(&EdDelay));
            break;
        }

        case EID_Ku:
        case EID_Att:
        case EID_StStr:
        case EID_EndStr:
        case EID_HeadPh:
        case EID_TVG:
        {
            tEventByteData data;
            _pFile->read(reinterpret_cast<char*>(&data), sizeof(tEventByteData));
            break;
        }
        case EID_ChangeOperatorName:
        {
            unsigned char strSize = '\0';
            _pFile->getChar(reinterpret_cast<char*>(&strSize));
            const QByteArray& data = _pFile->read(128);
            break;
        }
        case EID_CheckSum:
        {
           _pFile->seek(_pFile->pos() + 4);
           break;
        }
        case EID_DEBUG:
        {
            _pFile->seek(_pFile->pos() + 128);
            break;
        }
        case EID_BigDate:
        {
            const QByteArray& data = _pFile->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            _pFile->seek(_pFile->pos() + 1 + dataSize);
            break;
        }
        case EID_DefLabel:
        {
            unsigned char length = '\0';
            _pFile->getChar(reinterpret_cast<char*>(&length));
            _pFile->seek(_pFile->pos() + 1);
            const QByteArray& data = _pFile->read(length * 2);
            break;
        }
        case EID_HandScan:
        {
            const QByteArray& data = _pFile->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            _pFile->seek(_pFile->pos() + 7 + dataSize);
            break;
        }
        case EID_PrismDelay:
        {
            tEventWordData data;
            _pFile->read(reinterpret_cast<char*>(&data), sizeof(tEventWordData));
            break;
        }
//
        case EID_Media:
        {
            _pFile->seek(_pFile->pos() + 1);
           const QByteArray& data = _pFile->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            _pFile->seek(_pFile->pos() + dataSize);
            break;
        }
        case EID_MediumDate:
        {
            const QByteArray& data = _pFile->read(2);
            quint16 dataSize = qFromLittleEndian<quint16>(reinterpret_cast<const unsigned char*>(data.data()));
            _pFile->seek(_pFile->pos() + 1 + dataSize);
            break;
        }
        case EID_Mode:
        {
           unsigned char data8[8];
            _pFile->read(reinterpret_cast<char*>(&data8[1]), 7);
            break;
        }
        case EID_NORDCO_Rec:
        {
            _pFile->seek(_pFile->pos() + 2015);
            break;
        }
        case EID_SatelliteCoordAndSpeed:
        {
            _pFile->seek(_pFile->pos() + 12);
            break;
        }
        case EID_SCReceiverStatus:
        {
            unsigned char data10[10];
            _pFile->read(reinterpret_cast<char*>(&data10[1]), 9);
            break;
        }
        case EID_SensAllowedRanges:
        {
        unsigned char count = '\0';
        char data768[256 * 3];
            _pFile->getChar(reinterpret_cast<char*>(&count));
            _pFile->read(reinterpret_cast<char*>(&data768[0]), count * 3);
            break;
        }
        case EID_SensFault:
        {
           unsigned char data7[7];
           _pFile->read(reinterpret_cast<char*>(&data7[1]), 6);

            break;
        }
        case EID_SpeedState:
        {
        unsigned char data6[6];
         _pFile->read(reinterpret_cast<char*>(&data6[1]), 5);
            break;
        }
        case EID_Stolb:
        {
            if ((pParsedData) && (Id == EID_Stolb))
            {
                _pFile->read(reinterpret_cast<char*>(pParsedData), sizeof(sCoordPostMRF));
            }
                else
                {
                    unsigned char data145[145];
                    _pFile->read(reinterpret_cast<char*>(&data145[1]), sizeof(data145) - 1);
                }
            break;
        }
//
        case EID_Switch:
        case EID_TextLabel:
        {
        unsigned char length = '\0';
            _pFile->getChar(reinterpret_cast<char*>(&length));
            const QByteArray& data = _pFile->read(length * 2);
            break;
        }
        case EID_EndFile:
        {
            const QByteArray& data = _pFile->read(13);
            if (data != QByteArray().fill(0xFF, 13)) {
                res = false;
            }
            break;
        }
        case EID_LongLabel:
        {
            _pFile->seek(_pFile->pos() + 24);
            break;
        }
        case EID_SetRailType:
        case EID_StBoltStyk:
        case EID_EndBoltStyk:
        case EID_StartSwitchShunter:
        case EID_EndSwitchShunter:
        {

            break;
        }

        case EID_OperatorRemindLabel:
        {
            unsigned char textLen = '\0';
            _pFile->getChar(reinterpret_cast<char*>(&textLen));
            _pFile->seek(_pFile->pos() + 10 + 2 * textLen);
        }
        case EID_PaintSystemRes_Debug:
        {
            _pFile->seek(_pFile->pos() + 246);
            break;
        }
        case EID_PaintMarkRes:
        case EID_SatelliteCoord:
        {
            _pFile->seek(_pFile->pos() + 8);
            break;
        }
        case EID_PaintSystemParams:
        {
            _pFile->seek(_pFile->pos() + 2048);
            break;
        }
        case EID_PaintSystemRes:
        {
            _pFile->seek(_pFile->pos() + 182);
            break;
        }
        case EID_AlarmTempOff:
        case EID_PaintSystemTempOff:
        case EID_Temperature:
        {
            _pFile->seek(_pFile->pos() + 5);
            break;
        }
        case EID_QualityCalibrationRec:
        {
            _pFile->seek(_pFile->pos() + 15);
            break;
        }
        case EID_SmallDate:
        {
            unsigned char dataSize = '\0';
            _pFile->getChar(reinterpret_cast<char*>(&dataSize));
            _pFile->seek(_pFile->pos() + 1 + dataSize);
            break;
        }
        case EID_StolbChainage:
        {
            _pFile->seek(_pFile->pos() + 136);
            break;
        }
        case EID_SysCrd_NS:
        {
        unsigned int *pCoord;
            unsigned char byte = '\0';
            pCoord = reinterpret_cast<unsigned int*>(pParsedData);
            _pFile->getChar(reinterpret_cast<char*>(&byte));
            if ((pParsedData) && (Id == EID_SysCrd_NS))
            {
                *pCoord = byte & 0xFF;
            }
//        int difference = _coordinate - systemCoordinate;
//        if(std::abs(difference) > 1){
//            qDebug() << "invalid difference: " + QString::number(difference);
//            difference = 1;

            break;
        }
        case EID_SysCrd_NF:
        { //Полная системная координата без ссылки
        unsigned int *pCoord;
            pCoord = reinterpret_cast<unsigned int*>(pParsedData);
            const QByteArray& data = _pFile->read(4);
            if ((pParsedData) && (Id == EID_SysCrd_NF))
            {
                *pCoord = qFromLittleEndian<int>(reinterpret_cast<const unsigned char*>(data.data()));
            }
            break;
        }
        case EID_Time:
        case EID_Sensor1:
        {
            _pFile->seek(_pFile->pos() + 2);
            break;
        }
        case EID_UMUPaintJob:
        {
            _pFile->seek(_pFile->pos() + 9);
            break;
        }
        case EID_ZerroProbMode:
        case EID_ACState:
        {
            _pFile->seek(_pFile->pos() + 1);
            break;
        }
        default:
        {
            res = false;
            assert(0);
        }
    }
    }
        else
        {
        tDaCo_BScanSignals *pSignal;
//        tDaCo_BScanSignals currentSignal;
        unsigned char byte;
            if (pParsedData)
            {
                pSignal = (static_cast<tDaCo_BScanSignals*>(pParsedData));
            }
                else
                {
                    pSignal = (static_cast<tDaCo_BScanSignals*>(&currentSignal));
                }
            {
                pSignal->Side = static_cast<eDeviceSide>(convertToUMUSide((ID[0] & 0x40) >> 6));  // Сторона БУМ - eUMUSide
                pSignal->Channel = (ID[0] >> 2) & 0xF;  // Канал
                pSignal->Count = (ID[0] & 0x3) + 1;      // Количество сигналов
                switch(pSignal->Count)
                {
                    case 1:
                    {
                        _pFile->getChar(reinterpret_cast<char*>(&pSignal->Signals[0].Delay));
                        _pFile->getChar(reinterpret_cast<char*>(&byte));
                        pSignal->Signals[0].Ampl = byte >> 4;
                        break;
                    }
                    case 2:
                    {
                       _pFile->getChar(reinterpret_cast<char*>(&pSignal->Signals[0].Delay));
                       _pFile->getChar(reinterpret_cast<char*>(&pSignal->Signals[1].Delay));
                       _pFile->getChar(reinterpret_cast<char*>(&byte));
                       pSignal->Signals[0].Ampl = byte >> 4;
                       pSignal->Signals[1].Ampl = byte & 0xF;
                       break;
                    }
                    case 3:
                    {
                       _pFile->getChar(reinterpret_cast<char*>(&pSignal->Signals[0].Delay));
                       _pFile->getChar(reinterpret_cast<char*>(&pSignal->Signals[1].Delay));
                       _pFile->getChar(reinterpret_cast<char*>(&pSignal->Signals[2].Delay));
                       _pFile->getChar(reinterpret_cast<char*>(&byte));
                       pSignal->Signals[0].Ampl = byte >> 4;
                       pSignal->Signals[1].Ampl = byte & 0xF;
                       _pFile->getChar(reinterpret_cast<char*>(&byte));
                       pSignal->Signals[2].Ampl = byte >> 4;
                       break;
                    }
                    default:
                    {
                        _pFile->getChar(reinterpret_cast<char*>(&pSignal->Signals[0].Delay));
                        _pFile->getChar(reinterpret_cast<char*>(&pSignal->Signals[1].Delay));
                        _pFile->getChar(reinterpret_cast<char*>(&pSignal->Signals[2].Delay));
                        _pFile->getChar(reinterpret_cast<char*>(&pSignal->Signals[3].Delay));
                        _pFile->getChar(reinterpret_cast<char*>(&byte));
                        pSignal->Signals[0].Ampl = byte >> 4;
                        pSignal->Signals[1].Ampl = byte & 0xF;
                        _pFile->getChar(reinterpret_cast<char*>(&byte));
                        pSignal->Signals[2].Ampl = byte >> 4;
                        pSignal->Signals[3].Ampl = byte & 0xF;
                        break;
                   }
                }
            }
        }
    Id = ID[0];
    _fileOffset = _pFile->pos();
    return res;
}
// перевод длины в шагах ДП в длину в мм
unsigned int Test::convertSystemCoordToMM(unsigned int steps)
{
    return static_cast<unsigned int>(steps * _Header.ScanStep / 100.0);
}
// перевод длины в мм в длину в шагах ДП
unsigned int Test::convertMMToSystemCoord(unsigned int mm)
{
unsigned long long res;
    res = mm * 100 / _Header.ScanStep;
    return static_cast<unsigned int>(res);
}
//файл должен быть открыт
unsigned int Test::countCoordUntilFileOffet(unsigned int objectId, qint64 fileOffset)
{
unsigned int res = 0;
bool fShort;
OBJECTLIB lib;
tObjectSourceDescr descriptor;

   if(lib.getRecordData(objectId, descriptor))
   {
    if (openFile(descriptor.SourceFileName))
    {
        if (readHeader())
        {
            while (_fileOffset < fileOffset) {
                if (readNextCoord(res, fShort) == false) break;
            }
        }
    }
   }
    return res;
}

unsigned char Test::getSideByte(eUMUSide Side)
{
    return (Side == usRight) ? 0x01 : 0x00;
}

eUMUSide Test::convertToUMUSide(unsigned char sideByte)
{
    assert((sideByte == 0) || (sideByte == 1));
    return (sideByte == 0x01) ? usRight : usLeft;
}


bool Test::footerExist() {
    return _Header.TableLink != 0xFFFFFFFF;
}
