// в проекте QT определить DEVICE_EMULATION, AC_dis

#include <stdlib.h>
#include <QDebug>
#include <time.h>
#include <math.h>
#include <QDateTime>
#include "platforms.h"
#include "ChannelsIds.h"
#include "MISC133.H"
#include "umudevice.h"
#include "variety.h"


class UMUDEVICE* UMUDEVICE::deviceObjectPtr;

// parentClass - указатель на объект либо UNITLIN, либо UNITWIN
UMUDEVICE::UMUDEVICE(cThreadClassList* ThreadClassList, void *parentClass, CONFIG *pConfig): _pConfig(pConfig)
{
cDataTransferLan::cLanConnectionParams connectionParams;
cCriticalSection *pCS1;
cCriticalSection *pCS2;

     _parentClass = reinterpret_cast<UNITLIN*>(parentClass);
    _movingDirection = Test::DirNotDefined;
    qsrand(getCurrentTime());
     _pTrolley = new TROLLEY(createCriticalSection());
     pCS1 = createCriticalSection();
     pCS2 = createCriticalSection();
    _pEmulator = new EMULATOR(pCS1, pCS2, TROLLEY::step, _pConfig->getPathToObjectsFiles());
    _pEmulator->getChannelList(_channelList);
    connect(_pEmulator, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
//
     _thlist = ThreadClassList;

     _critical_section[CDUoutBufferIndex] = createCriticalSection();
     _critical_section[PCoutBufferIndex] = createCriticalSection();

     setPLDInterruptEnable(false);

     _pldl = new PLDEMULATOR(createCriticalSection());
     _pldr = new PLDEMULATOR(createCriticalSection());
     _umu = new UMU(this);
     _umu->moveLargeBScanInit();
     _umu->ush_init();

     connect(_pTrolley, SIGNAL(pathStep(int, int, int)), this, SLOT(_onPathStep(int, int, int)));

     _PLDIntTimer.setInterval(1);
     connect(&_PLDIntTimer, SIGNAL(timeout()), this, SLOT(_onPLDInt()));
     _PLDIntTimer.start();
//
    _receiveStartOffset = 0;
    _write_error_count[CDUoutBufferIndex] = 0;
    _write_error_count[PCoutBufferIndex] = 0;

    _endWorkFlag = false;
    _restartCDUConnectionFlag = false;

    _dtLan = new (std::nothrow) cDataTransferLan;
// БУИ-соединение
    _CDUConnected = false;

    QByteArray IPAddress;
    IPAddress = getCDULocalIPAddress().toLatin1();
    strncpy(connectionParams._local_ip, IPAddress.data(), cDataTransferLan::cLanConnectionParams::_ip_size);

    IPAddress = getCDURemoteIPAddress().toLatin1();
    strncpy(connectionParams._remote_ip, IPAddress.data(), cDataTransferLan::cLanConnectionParams::_ip_size);

    connectionParams._port_in = getCDULocalPort(); // 43000;
    connectionParams._port_out = getCDURemotePort(); // 43001;
    connectionParams._socket_1_tcp = true;
    connectionParams._socket_2_tcp = false;
    connectionParams._socket_1_server = true;
    connectionParams._socket_1_transfer_direction = cSocketLan::DirectionToLocal;

#ifdef _test_message_numeration_integrity
    _messageNumber = 0;
    _lastMessageID = 0;
#endif

    _CDUConnection_id = _dtLan->addConnection(reinterpret_cast<cDataTransferLan::cLanConnectionParams* const>(&connectionParams));
//
// ПК-соединение
    _PCConnected = false;

    IPAddress = getPCLocalIPAddress().toLatin1();
    strncpy(connectionParams._local_ip, IPAddress.data(), cDataTransferLan::cLanConnectionParams::_ip_size);
    IPAddress = getPCRemoteIPAddress().toLatin1();
    strncpy(connectionParams._remote_ip, IPAddress.data(), cDataTransferLan::cLanConnectionParams::_ip_size);

    connectionParams._port_in =  getPCLocalPort(); //50002  local
    connectionParams._port_out = getPCRemotePort();//  50002 remote

    connectionParams._socket_1_tcp = true;
    connectionParams._socket_2_tcp = true;
    connectionParams._socket_1_server = false;
    connectionParams._socket_1_transfer_direction = cSocketLan::DirectionBoth;

    _PCConnection_id = _dtLan->addConnection(reinterpret_cast<cDataTransferLan::cLanConnectionParams* const>(&connectionParams));
//
    _read_state = rsHead;
    _error_message_count = 0;

    deviceObjectPtr = this;

    _pPingTimerCS = createCriticalSection();

    _needToPing = false;
     connect(&_pingTimer, SIGNAL(timeout()), this, SLOT(_onPingTimer()));
    _pingTimer.start(PING_PERIOD);


    _PCLinkFault = false;
     connect(&_PCLinkFaultTimer, SIGNAL(timeout()), this, SLOT(_onPCLinkFaultTimer()));
    _PCLinkFaultTimer.setInterval(PC_LINK_FAULT_TIMEOUT);
    _PCLinkFaultTimer.start();
     connect(this, SIGNAL(restartPCLinkFaultTimer()), this, SLOT(_onRestartPCLinkFaultTimer()));

     QString logFilePath = _pConfig->getPathToObjectsFiles();
     QString logFileName;
#ifdef PATH_MAP_LOGFILE_ON
    logFileName = "pathmap.txt";
    _pPathMapLogFile = new LOGFILE(&logFilePath, &logFileName);
#else
    _pPathMapLogFile = 0;
#endif
#ifdef PCLAN_MESSAGE_LOGFILE_ON
    logFileName = "lanmessages.txt";
    _pLANPCMessageLogFile = new LOGFILE(&logFilePath, &logFileName);
#else
    _pLANPCMessageLogFile = 0;
#endif

    _engineThreadIndex = _thlist->AddTick(DEFCORE_THREAD_FUNCTION(UMUDEVICE, this, engine));

    _UmuTickThreadIndex = 0;
    _thlist->Resume(_thlist->AddTick(DEFCORE_THREAD_FUNCTION(UMUDEVICE, this, CDUTick)));
    _thlist->Resume(_thlist->AddTick(DEFCORE_THREAD_FUNCTION(UMUDEVICE, this, PCTick)));
}
//
UMUDEVICE::~UMUDEVICE()
{
    deviceObjectPtr = nullptr;
    delete _pldl;
    delete _pldr;
    delete _pTrolley;
    if (_pPathMapLogFile != 0) delete _pPathMapLogFile;
    if (_pLANPCMessageLogFile != 0) delete _pLANPCMessageLogFile;
}

void UMUDEVICE::start(void)
{
    _thlist->Resume(_engineThreadIndex);
}

void UMUDEVICE::stop(void)
{
    _endWorkFlag = true;
    SLEEP(2000);
}

bool UMUDEVICE::engine(void)
{
    SLEEP(1);
#ifndef SKIP_CDU_CONNECTING
   if (_CDUConnected == false)
   {
       if (_dtLan->openConnection(_CDUConnection_id) == 0)
       {
           _restartCDUConnectionFlag = false;
           _CDUConnected = true;
           qWarning() << "CDU connected";
           if(_UmuTickThreadIndex == 0)
           {
               _UmuTickThreadIndex = _thlist->AddTick(DEFCORE_THREAD_FUNCTION(UMUDEVICE, this, umuTick));
               _thlist->Resume(_UmuTickThreadIndex);
           }
       }
   }
   else {
       if(_restartCDUConnectionFlag)
       {
           _restartCDUConnectionFlag = false;
           qWarning() << "CDU reconnection...";
           _dtLan->closeConnection(_CDUConnection_id);
           _CDUConnected = false;
        }
    }
#endif
//
#ifndef SKIP_PC_CONNECTING
    if (_PCConnected == false)
    {
        if (_dtLan->openConnection(_PCConnection_id) == 0)
        {
            _PCLinkFault = false;
            _PCConnected = true;
            qWarning() << "PC connected";
        }
    }
        else {
            if ((_pConfig->getRestorePCConnectionFlagState()) && (_PCLinkFault))
            {
                qWarning() << "PC LAN reconnection due to ping timeout";
                _dtLan->closeConnection(_PCConnection_id);
                _PCConnected = false;
                _read_state = rsHead;
            }
        }
#endif
    return !_endWorkFlag;
}

//
bool UMUDEVICE::CDUTick(void)
{
    CDUTickSend();
    if (_CDUConnected) this->TickCDUReceive();
    SLEEP(1);
    return !_endWorkFlag;
}
//
bool UMUDEVICE::PCTick(void)
{
    _pPingTimerCS->Enter();
    if ((_PCConnected) && (_needToPing))
    {
        sendPingToPC();
        _needToPing = false;
    }
    _pPingTimerCS->Release();

    PCTickSend();
    if (_PCConnected) this->TickPCReceive();

    SLEEP(1);
    return !_endWorkFlag;
}
//
void UMUDEVICE::CDUTickSend()
{
    if (_CDUConnected) unload(CDUoutBufferIndex);
}
//
void UMUDEVICE::PCTickSend()
{
    if (_PCConnected) unload(PCoutBufferIndex);
}
//
void UMUDEVICE::unload(eOutBufferIndex outBufferIndex)
{
bool res;
    DEFCORE_ASSERT((outBufferIndex == CDUoutBufferIndex) || (outBufferIndex == PCoutBufferIndex));
    _critical_section[outBufferIndex]->Enter();
    switch(outBufferIndex)
    {
       case CDUoutBufferIndex:
       {
           while (!_CDU_out_buffer.empty()) {
               const tLAN_CDUMessage& currentMessage = _CDU_out_buffer.front();
               res = _dtLan->write(_CDUConnection_id, reinterpret_cast<const unsigned char*>(&currentMessage), currentMessage.Size + LAN_MESSAGE_BIG_HEADER_SIZE);
               if (res) _CDU_out_buffer.pop();
                   else
                   {
                       _write_error_count[outBufferIndex]++;
#ifdef UMUOutDataLog
                       Log->AddText(" - WriteToLAN - Error");
#endif
                       break;
                   }
           }
           break;
       }
       default:
           while(!_PC_out_buffer.empty()) {
               const tLAN_PCMessage& currentMessage = _PC_out_buffer.front();
               res = _dtLan->write(_PCConnection_id, reinterpret_cast<const unsigned char*>(&currentMessage), currentMessage.Size + LAN_MESSAGE_SHORT_HEADER_SIZE);
               if (res) _PC_out_buffer.pop();
                   else
                   {
                       _write_error_count[outBufferIndex]++;
#ifdef UMUOutDataLog
                       Log->AddText(" - WriteToLAN - Error");
#endif
                       break;
                   }
           }
           break;
    }
    _critical_section[outBufferIndex]->Release();
}
//
void UMUDEVICE::TickCDUReceive()
{
int res;
    res = _dtLan->read(_CDUConnection_id, &_incmdbuf[_receiveStartOffset], UIP_BUFSIZE - _receiveStartOffset);
    if (res != 0)
    {
        _receiveStartOffset = _umu->lanmsgparcer(_incmdbuf, (unsigned short)(res + _receiveStartOffset));
    }
}
//
void UMUDEVICE::TickPCReceive()
{
bool fRepeat;
    do {
        fRepeat = false;
        switch (_read_state) {
        case rsHead: {
            readPCMessageHead(fRepeat);
            if (fRepeat)
            {
                _read_state = rsTestHead;
            }
            break;
        }
        case rsBody: {
            readPCMessageBody(fRepeat);
            if (fRepeat)
            {
                if (_pLANPCMessageLogFile != 0)
                {
                    QStringList prnStrings;
                    QStringList::iterator it;
                    prnStrings = _currentMessage.printBody();
                    if(!prnStrings.isEmpty())
                    {
                        for (it=prnStrings.begin(); it != prnStrings.end(); ++it)
                        {
                            _pLANPCMessageLogFile->addNote(*it);
                        }
                    }
                }
                _read_state = rsHead;
                unPack(_currentMessage);
                _currentMessage.resetMessage();
            }
            break;
        }
        case rsTestHead: {
            fRepeat = true;
            if (_currentMessage.messageCorrectness() == true) {
                if (_pLANPCMessageLogFile != 0)
                {
                    QString prnString = _currentMessage.printHeader();
                    _pLANPCMessageLogFile->startBlock();
                    _pLANPCMessageLogFile->addNote(prnString);
                }
                _read_state = rsBody;
            }
                else {
                    _read_state = rsSkipWrongId;
                }
            break;
        }
        case rsSkipWrongId:{
            unsigned char wrongId = skipWrongMessageId(fRepeat);
            if (fRepeat)
            {// wrongId - содержит неверный Id
             // считали один байт из потока
                if (_pLANPCMessageLogFile != 0)
                {
                    QString note = QString::asprintf("Skipped: 0x%x", wrongId);
                    _pLANPCMessageLogFile->startBlock();
                    _pLANPCMessageLogFile->addNote(note);
                }
                _read_state = rsTestHead;
            }
            break;
        }
        case rsOff: break;
        }
    } while (fRepeat);
}
//
bool UMUDEVICE::umuTick()
{
    _umu->ustsk(0);
    SLEEP(1);
    return !_endWorkFlag;
}

#ifdef _test_message_numeration_integrity
void UMUDEVICE::testMessageNumerationIntegrity(tLAN_CDUMessage* _out_block)
{
    if (_out_block->MessageCount != _messageNumber)
    {
        emit message(QString::asprintf("ERROR: real LAN-messsage to CDU number = %d, expected number = %d, this message ID = 0x%x, last message ID = 0x%x", _out_block->MessageCount, _messageNumber, _out_block->Id, _lastMessageID));
    }
    _messageNumber = _out_block->MessageCount + 1;
    _lastMessageID = _out_block->Id;
}
#endif

void UMUDEVICE::writePLDRegister(eUMUSide side, unsigned int regAddress, unsigned char value)
{
    if (side == usLeft)
    {
        _pldl->writeRegister(regAddress, value);
    }
        else
        {
            _pldr->writeRegister(regAddress, value);
        }
}

unsigned char UMUDEVICE::readPLDRegister(eUMUSide side, unsigned int regAddress)
{
    if (side == usLeft)
    {
        return _pldl->readRegister(regAddress);
    }
    return _pldr->readRegister(regAddress);
}

void UMUDEVICE::writeIntoRAM(eUMUSide side, unsigned int regAddress, unsigned char value)
{
    if (side == usLeft)
    {
        _pldl->writeIntoRAM(regAddress, value);
    }
        else
        {
            _pldr->writeIntoRAM(regAddress, value);
        }
}

unsigned char UMUDEVICE::readFromRAM(eUMUSide side, unsigned int regAddress)
{
    if (side == usLeft)
    {
        return _pldl->readFromRAM(regAddress);
    }
    return _pldr->readFromRAM(regAddress);
}

void UMUDEVICE::readPCMessageHead(bool& done)
{
unsigned int res;
    res = _dtLan->read(_PCConnection_id, reinterpret_cast<unsigned char*>(&_currentMessage), LAN_MESSAGE_SHORT_HEADER_SIZE);

    if (res == LAN_MESSAGE_SHORT_HEADER_SIZE) {
        done = true;
        _read_bytes_count = _currentMessage.Size;

/*
        _common_state._message_read_bytes_count += _currentMessage.Size;

        if (_special_state._last_message_count != -1) {
            if (!((_currentMessage.MessageCount - _special_state._last_message_count == 0x01) || ((_currentMessage.MessageCount == 0x00) && (_special_state._last_message_count == 0xFF)))) {
                if (_currentMessage.MessageCount > _special_state._last_message_count) {
                    _special_state._skip_message_count += _currentMessage.MessageCount - _special_state._last_message_count - 1;
                }
                else if (_currentMessage.MessageCount < _special_state._last_message_count) {
                    _special_state._skip_message_count += _currentMessage.MessageCount + 255 - _special_state._last_message_count;
                }
                else if (_currentMessage.MessageCount == _special_state._last_message_count) {
                    _special_state._skip_message_count += 255;
                }
            }
        }

        _special_state._last_message_count = _currentMessage.MessageCount;
        ++_common_state._message_count;
    }
    else if (res != 0) {
        _common_state._error_message_count++;
#ifdef DbgLog
        if (useLog) onAddLog(_common_state._umu_id, NULL, 3);  // Ошибка приема данных
#endif
*/
    }
}

void UMUDEVICE::readPCMessageBody(bool& done)
{
unsigned int res = 0;
    DEFCORE_ASSERT(_read_bytes_count <= tLAN_PCMessage::LanDataMaxSize);
    if (_read_bytes_count != 0)
    {// если сообщение содержит данные
        res = _dtLan->read(_PCConnection_id, reinterpret_cast<unsigned char*>(&_currentMessage.Data), _read_bytes_count);
    }
    if (res == _read_bytes_count) {
        done = true;
#ifdef DbgLog
        if (useLog) onAddLog(_common_state._umu_id, (unsigned char*) &_currentMessage, 1);
#endif
    }
        else if (res != 0) {
        ++_error_message_count;
#ifdef DbgLog
        if (useLog) onAddLog(_common_state._umu_id, (unsigned char*) &_currentMessage, 3);  // Ошибка приема данных
#endif
    }
}

// читает из потока входных данных 1 байт, если он принят,
//выкидывает неправильный идентификатор сообщения и возвращает его значение, при этом done == true
unsigned char UMUDEVICE::skipWrongMessageId(bool& done)
{
unsigned char res = 0;
unsigned int byteCount = 0;
unsigned char byte;
    byteCount = _dtLan->read(_PCConnection_id, &byte, 1);
    if (byteCount)
    {
        done = true;
        res = _currentMessage.Id;
        _currentMessage.Id = _currentMessage.Reserved;
        _currentMessage.Reserved = static_cast<unsigned char>(_currentMessage.Size);
        _currentMessage.Size = (_currentMessage.Size >> 8) | (byte << 8);
    }
    return res;
}

void UMUDEVICE::unPack(tLAN_PCMessage &buff)
{
    switch (buff.Id)
    {
        case PingId:
        {
            emit restartPCLinkFaultTimer();
            break;
        }
//
        case TrackMapId:
        {
        unsigned short IdCount;
        unsigned char *bytePtr;
        unsigned int byteCount;
        int coord;
        unsigned short id;
        QString messageString;

            bytePtr = &buff.Data[0];
            int temp;
            temp = (int)ReadLE32U(bytePtr);
            switch(temp)
            {
                case 1:
                {
                    _movingDirection = Test::DirUpWard;
                    break;
                }
                case -1:
                {
                    _movingDirection = Test::DirDownWard;
                    break;
                }
            default:
                qWarning() << "unPack: wrong field movingDirection in the message TrackMapId. Message was ignored";
                return;
            }
            _pEmulator->setMovingDirection(_movingDirection);
            _pTrolley->setMovingDirection(_movingDirection);
            _pEmulator->deletePathObjects();
            bytePtr += sizeof(int32_t);

            byteCount = static_cast<int>(buff.Size - sizeof(int32_t));

            if (_pPathMapLogFile != 0)
            {
                _pPathMapLogFile->startBlock();
#ifdef TESTING_PATH_MAP_ON
                connect(_pEmulator, SIGNAL(message(QString)), _pPathMapLogFile, SLOT(addNote(QString)));
#endif
            }

            for (unsigned int jj=0; jj < 2; ++jj)
            {
                if (byteCount == 0) break;
                IdCount = ReadLE16U (bytePtr);
                if (byteCount < IdCount * (sizeof(coord) + sizeof(id) )) break;

//                if (jj == 0) messageString = QString::asprintf("left side flaw count = %d", IdCount);
//                    else messageString = QString::asprintf("right side flaw count = %d", IdCount);
//                qWarning() << messageString;

                bytePtr += sizeof(unsigned short);
                for (unsigned short ii=0; ((ii < IdCount) && (byteCount > 0)); ++ii)
                {
                    coord = ReadLE32U (bytePtr);
                    bytePtr += sizeof(coord);
                    id = ReadLE16U (bytePtr);
                    bytePtr += sizeof(id);
                    if (jj == 0)
                    {// данные для левой стороны
                      bool res;

                        res = _pEmulator->onMessageId(id, coord, usLeft);
                        if (res) messageString = QString::asprintf("left  side flaw: id = %d, coord = %d", id, coord);
                            else messageString = QString::asprintf("left  side flaw: id = %d, coord = %d - ignored", id, coord);
                    }
                       else
                       {
                           bool res;
                           res = _pEmulator->onMessageId(id, coord, usRight);
                           if (res) messageString = QString::asprintf("right side flaw: id = %d, coord = %d", id, coord);
                               else messageString = QString::asprintf("right side flaw: id = %d, coord = %d - ignored", id, coord);
                       }
//                        qWarning() << messageString;
                      if (_pPathMapLogFile != 0)
                      {
                          _pPathMapLogFile->addNote(messageString);
                      }
                }
                byteCount -= IdCount * (sizeof(coord) + sizeof(id));
            }
#ifdef TESTING_PATH_MAP_ON
            _pEmulator->testPathMap();
            if (_pPathMapLogFile != 0)
            {
                disconnect(_pEmulator, SIGNAL(message(QString)), _pPathMapLogFile, SLOT(addNote(QString)));
            }
#endif
            break;
        }
//
        case NextTrackCoordinateId:
        {
            tNEXTTRACKCOORD *pMessage;
            if (buff.Size == sizeof(tNEXTTRACKCOORD))
            {
                pMessage = reinterpret_cast<tNEXTTRACKCOORD*>(buff.Data);

                if (fabs(pMessage->Speed) > TROLLEY::AbsMaxV * 1100.0) // сделаем допуск +-10%
                {
                    qWarning() << "unPack: Wrong value of speed" << pMessage->Speed << "was limited";
                    if (pMessage->Speed > 0.0)
                    {
                        pMessage->Speed = TROLLEY::AbsMaxV * 1100.0;
                    }
                        else
                        {
                            pMessage->Speed = TROLLEY::AbsMaxV * (-1100.0);
                        }
                }
                _pTrolley->changeMovingParameters(pMessage->Speed, pMessage->Coord, pMessage->LeftDebugCoord, pMessage->RightDebugCoord, pMessage->Time);
            }
            break;
        }
//
        case JumpTrackCoordinateId:
        {
         tJUMPTRACKCOORD *pMessage;
            if (buff.Size == sizeof(tJUMPTRACKCOORD))
            {
               pMessage = reinterpret_cast<tJUMPTRACKCOORD*>(buff.Data);
              _pEmulator->testCoordinate(pMessage->Coord);
              _pTrolley->setCoordinate(pMessage->Coord, pMessage->LeftDebugCoord, pMessage->RightDebugCoord);
            }
            break;
        }
    }
}
//
void UMUDEVICE::AddToOutBuffSync(tLAN_PCMessage* _out_block)
{
    _critical_section[PCoutBufferIndex]->Enter();
    _PC_out_buffer.push(*_out_block);
    _critical_section[PCoutBufferIndex]->Release();
}
//
void UMUDEVICE::AddToOutBuffNoSync(tLAN_PCMessage* _out_block)
{
    _PC_out_buffer.push(*_out_block);
}

void UMUDEVICE::NonSyncAddToOutBuffer(tLAN_CDUMessage* _out_block)
{
#ifdef _test_message_numeration_integrity
    testMessageNumerationIntegrity(_out_block);
#endif
    _CDU_out_buffer.push(*_out_block);
}

void UMUDEVICE::NonSyncAddToOutBufferLock()
{
    _critical_section[CDUoutBufferIndex]->Enter();
}

void UMUDEVICE::NonSyncAddToOutBufferUnlock()
{
    _critical_section[CDUoutBufferIndex]->Release();
}

void UMUDEVICE::_onPCLinkFaultTimer()
{
    if(_PCConnected)
    {
        _PCLinkFault = true;
    }
}

void UMUDEVICE::_onRestartPCLinkFaultTimer()
{
    _PCLinkFaultTimer.start();
}

void UMUDEVICE::_onPingTimer()
{
    _pPingTimerCS->Enter();
    _needToPing = true;
    _pPingTimerCS->Release();
}

void UMUDEVICE::sendPingToPC()
{
tLAN_PCMessage m;
    m.Id = PingId;
    m.Size = 0;
   AddToOutBuffSync(&m);
}

/*
void UMUDEVICE::sendResponseWithData(const unsigned char id, const unsigned char* data, unsigned short size)
{
tLAN_CDUMessage m;

     memcpy(&m.Data, data, size);
     m.Id = id;
     m.Size = size;
#ifndef SHORT_HEADER
     m.MessageCount = 0;
     m.NotUse = 0;
#endif
     AddToOutBuffSync(&m);
}
*/
//
// срабатывание таймера _PLDIntTimer
void UMUDEVICE::_onPLDInt()
{
    if (_enablePLDInt)
    {
        _umu->PLDInterruptEmulation();
    }
}

// слот на сигнал о срабатывании ДП от trolley
void UMUDEVICE::_onPathStep(int shift, int coordLInMM, int coordRInMM)
{
    whenTrolleyCoordChanged(coordLInMM, coordRInMM);
    if (_movingDirection != Test::DirDownWard)
    {
        _pldl->setPathShft(shift);
    }
        else
        {
            _pldl->setPathShft(shift * -1);
        }
}

void UMUDEVICE::whenTrolleyCoordChanged(int coordLInMM, int coordRInMM)
{
SignalsData *pSignalsData;
 QList<CID>::iterator it;
 tSignalObject *pSignals;
 tStrokeConfig strokeAndLine;
 bool isDataObject; // данной координате соответствует объект, но нет сигналов

// предполагается, что координата coordInMM соответствует центру искательной системы
// получим координату ПЭП 0 гр.

    if (_movingDirection != Test::DirDownWard)
    {
        coordLInMM += N0EMS_SENSOR_SHIFT_mm;
        coordRInMM += N0EMS_SENSOR_SHIFT_mm;
    }
        else
        {
            coordLInMM -= N0EMS_SENSOR_SHIFT_mm;
            coordRInMM -= N0EMS_SENSOR_SHIFT_mm;
        }
//
    pSignalsData = _pEmulator->getScanObject(usLeft, coordLInMM, isDataObject);
    if (pSignalsData)
    {
        for (it=_channelList.begin(); it != _channelList.end(); ++it)
        {
            strokeAndLine = _pEmulator->CIDToLineAndStroke(*it, usLeft);
            pSignalsData->getSignals(*it, &pSignals);
            _pldl->resetSignals(strokeAndLine.Stroke, strokeAndLine.Line);
            if (pSignals != 0)
            {
                for (unsigned int ii=0; ii < pSignals->Count; ++ii)
                {
                  unsigned int delayMS;
                  unsigned int delayFrac;
                    if (*it != N0EMS)
                    {
                        delayMS = pSignals->Signals[ii].Delay;
                        delayFrac = 0;
                    }
                        else
                        {
                            delayMS = pSignals->Signals[ii].Delay / 3;
                            delayFrac = pSignals->Signals[ii].Delay % 3;
                        }
                    _pldl->addBScanSignal(strokeAndLine.Stroke, strokeAndLine.Line, delayMS, delayFrac, pSignals->Signals[ii].Ampl);
                }
            }
        }
    }
        else if (isDataObject == false)
             {
                 for (it=_channelList.begin(); it != _channelList.end(); ++it)
                 {
                     strokeAndLine = _pEmulator->CIDToLineAndStroke(*it, usLeft);
                     _pldl->resetSignals(strokeAndLine.Stroke, strokeAndLine.Line);
                 }
             }
//
    pSignalsData = _pEmulator->getScanObject(usRight, coordRInMM, isDataObject);
    if (pSignalsData)
    {
        for (it=_channelList.begin(); it != _channelList.end(); ++it)
        {
            strokeAndLine = _pEmulator->CIDToLineAndStroke(*it, usRight);
            pSignalsData->getSignals(*it, &pSignals);
            _pldr->resetSignals(strokeAndLine.Stroke, strokeAndLine.Line);
            if (pSignals != 0)
            {
                for (unsigned int ii=0; ii < pSignals->Count; ++ii)
                {
                    unsigned int delayMS;
                    unsigned int delayFrac;
                      if (*it != N0EMS)
                      {
                          delayMS = pSignals->Signals[ii].Delay;
                          delayFrac = 0;
                      }
                          else
                          {
                              delayMS = pSignals->Signals[ii].Delay / 3;
                              delayFrac = pSignals->Signals[ii].Delay % 3;
                          }
                    _pldr->addBScanSignal(strokeAndLine.Stroke, strokeAndLine.Line, delayMS, delayFrac, pSignals->Signals[ii].Ampl);
                }
            }
        }
    }
        else if (isDataObject == false)
        {
             for (it=_channelList.begin(); it != _channelList.end(); ++it)
             {
                 strokeAndLine = _pEmulator->CIDToLineAndStroke(*it, usRight);
                 _pldr->resetSignals(strokeAndLine.Stroke, strokeAndLine.Line);
             }
    }
}


unsigned int UMUDEVICE::getNumberOfTacts()
{
    return _pldl->getNumberOfTacts();
}

void UMUDEVICE::setPLDInterruptEnable(bool enabled)
{
    _enablePLDInt = enabled;
}

void UMUDEVICE::printConnectionStatus()
{
    if (_PCConnected)
    {
        qWarning() << "PC connection is present";
    }

    if (_CDUConnected)
    {
        qWarning() << "CDU connection is present";
    }
}

cCriticalSection *UMUDEVICE::createCriticalSection()
{
    return _parentClass->createCriticalSection();
}

void UMUDEVICE::criticalSectionEnter(cCriticalSection *pCS)
{
    _parentClass->criticalSectionEnter(reinterpret_cast <class cCriticalSection_Lin*>(pCS));
}

void UMUDEVICE::criticalSectionRelease(cCriticalSection *pCS)
{
    _parentClass->criticalSectionRelease(reinterpret_cast <class cCriticalSection_Lin*>(pCS));
}
//
QString& UMUDEVICE::getCDULocalIPAddress()
{
    return _pConfig->getCDULocalIPAddress();
}

QString& UMUDEVICE::getCDURemoteIPAddress()
{
    return _pConfig->getCDURemoteIPAddress();
}

bool UMUDEVICE::setCDULocalIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0)
{
    return _pConfig->setCDULocalIPAddress(IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
}

bool UMUDEVICE::setCDURemoteIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0)
{
    return _pConfig->setCDURemoteIPAddress(IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
}

QString& UMUDEVICE::getPCLocalIPAddress()
{
    return _pConfig->getPCLocalIPAddress();
}

QString& UMUDEVICE::getPCRemoteIPAddress()
{
    return _pConfig->getPCRemoteIPAddress();
}

bool UMUDEVICE::setPCLocalIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0)
{
    return _pConfig->setPCLocalIPAddress(IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
}

bool UMUDEVICE::setPCRemoteIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0)
{
    return _pConfig->setPCRemoteIPAddress(IPAddressPart3, IPAddressPart2, IPAddressPart1, IPAddressPart0);
}

bool UMUDEVICE::setCDULocalPort(QString port)
{
    return _pConfig->setCDULocalPort(port);
}

bool UMUDEVICE::setCDURemotePort(QString port)
{
    return _pConfig->setCDURemotePort(port);
}

bool UMUDEVICE::setPCLocalPort(QString port)
{
    return _pConfig->setPCLocalPort(port);
}

bool UMUDEVICE::setPCRemotePort(QString port)
{
    return _pConfig->setPCRemotePort(port);
}

unsigned short UMUDEVICE::getCDULocalPort()
{
    return _pConfig->getCDULocalPort();
}

unsigned short UMUDEVICE::getCDURemotePort()
{
    return _pConfig->getCDURemotePort();
}

unsigned short UMUDEVICE::getPCLocalPort()
{
    return _pConfig->getPCLocalPort();
}

unsigned short UMUDEVICE::getPCRemotePort()
{
    return _pConfig->getPCRemotePort();
}

bool UMUDEVICE::getRestorePCConnectionFlagState()
{
    return _pConfig->getRestorePCConnectionFlagState();
}

void UMUDEVICE::setRestorePCConnectionFlag(bool state)
{
    _pConfig->setRestorePCConnectionFlag(state);
}

QString UMUDEVICE::getPathToObjectsFiles()
{
    return _pConfig->getPathToObjectsFiles();
}

void UMUDEVICE::setPathToObjectsFiles(QString path)
{
    _pConfig->setPathToObjectsFiles(path);
}

bool UMUDEVICE::testPassword(QString& password)
{
    return _pConfig->testPassword(password);
}

void UMUDEVICE::save()
{
    _pConfig->save();
}

void UMUDEVICE::restartCDUConection()
{
    _restartCDUConnectionFlag = true;
}

void UMUDEVICE::onMessage(QString s) // слот на сигналы с текстовыми сообщениями от используемых классов
{
    emit message(s);
}

void UMUDEVICE::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg)
{
    QString txt;
    bool res;
    switch (type) {
        case QtInfoMsg:
            txt = QString("Info: %1").arg(msg);
        break;
        case QtDebugMsg:
            txt = QString("Debug: %1").arg(msg);
            break;
        case QtWarningMsg:
            txt = QString("Warning: %1").arg(msg);
        break;
        case QtCriticalMsg:
            txt = QString("Critical: %1").arg(msg);
        break;
        case QtFatalMsg:
            txt = QString("Fatal: %1").arg(msg);
            abort();
        break;
    }
    emit messageHandlerSignal(txt);
    QFile outFile(_pConfig->getPathToObjectsFiles() + "/qtMessages.log");
    res = outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    if (res)
    {
        QDateTime dateTime = QDateTime::currentDateTime();
        QTextStream ts(&outFile);
        txt = QString("%1:").arg(dateTime.toString(Qt::SystemLocaleShortDate)) + txt;
        ts << txt << endl;
        outFile.close();
    }
}
//----------------------------------------------------------------------------------------
const unsigned char UMU::mask[8] = {1,2,4,8,16,32,64,128};
const unsigned char UMU::mask2[8] = {0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};


// эмуляция параметров настройки БУМа, извлекаемых из файла PARAMS.INI

void UMU::dbgPrintOfMessage(tLAN_CDUMessage* _out_block)
{
    switch(_out_block->Size)
    {
        case 0:
        qWarning() << "LanMessage:"<< hex << _out_block->Id << _out_block->Source <<  _out_block->Size << "0" << \
                                         _out_block->MessageCount << _out_block->NotUse;
        break;
        case 1:
        qWarning() << "LanMessage:"<< hex << _out_block->Id << _out_block->Source <<  _out_block->Size << "0" << \
                                         _out_block->MessageCount << _out_block->NotUse << \
                                         _out_block->Data[0];
        break;
        case 2:
        qWarning() << "LanMessage:"<< hex << _out_block->Id << _out_block->Source <<  _out_block->Size << "0" << \
                                         _out_block->MessageCount << _out_block->NotUse << \
                                         _out_block->Data[0] << _out_block->Data[1];
        break;
        case 3:
        qWarning() << "LanMessage:"<< hex << _out_block->Id << _out_block->Source <<  _out_block->Size << "0" << \
                                         _out_block->MessageCount << _out_block->NotUse << \
                                         _out_block->Data[0] << _out_block->Data[1] << _out_block->Data[2];
        break;
        case 4:
        qWarning() << "LanMessage:"<< hex << _out_block->Id << _out_block->Source <<  _out_block->Size << "0" << \
                                         _out_block->MessageCount << _out_block->NotUse << \
                                         _out_block->Data[0] << _out_block->Data[1] << _out_block->Data[2] << _out_block->Data[3];
        break;
        case 5:
        qWarning() << "LanMessage:"<< hex << _out_block->Id << _out_block->Source <<  _out_block->Size << "0" << \
                                         _out_block->MessageCount << _out_block->NotUse << \
                                         _out_block->Data[0] << _out_block->Data[1] << _out_block->Data[2] << _out_block->Data[3] << \
                                         _out_block->Data[4];
        break;
        case 6:
        qWarning() << "LanMessage:"<< hex << _out_block->Id << _out_block->Source <<  _out_block->Size << "0" << \
                                         _out_block->MessageCount << _out_block->NotUse << \
                                         _out_block->Data[0] << _out_block->Data[1] << _out_block->Data[2] << _out_block->Data[3] << \
                                         _out_block->Data[4] << _out_block->Data[5];
        break;
        case 7:
        qWarning() << "LanMessage:"<< hex << _out_block->Id << _out_block->Source <<  _out_block->Size << "0" << \
                                         _out_block->MessageCount << _out_block->NotUse << \
                                         _out_block->Data[0] << _out_block->Data[1] << _out_block->Data[2] << _out_block->Data[3] << \
                                         _out_block->Data[4] << _out_block->Data[5] << _out_block->Data[6];
        break;
        case 8:
        qWarning() << "LanMessage:"<< hex << _out_block->Id << _out_block->Source <<  _out_block->Size << "0" << \
                                         _out_block->MessageCount << _out_block->NotUse << \
                                         _out_block->Data[0] << _out_block->Data[1] << _out_block->Data[2] << _out_block->Data[3] << \
                                         _out_block->Data[4] << _out_block->Data[5] << _out_block->Data[6] << _out_block->Data[7];
        break;
        case 9:
        qWarning() << "LanMessage:"<< hex << _out_block->Id << _out_block->Source <<  _out_block->Size << "0" << \
                                         _out_block->MessageCount << _out_block->NotUse << \
                                         _out_block->Data[0] << _out_block->Data[1] << _out_block->Data[2] << _out_block->Data[3] << \
                                         _out_block->Data[4] << _out_block->Data[5] << _out_block->Data[6] << _out_block->Data[7] << \
                                         _out_block->Data[8];
        case 10:
        qWarning() << "LanMessage:"<< hex << _out_block->Id << _out_block->Source <<  _out_block->Size << "0" << \
                                         _out_block->MessageCount << _out_block->NotUse << \
                                         _out_block->Data[0] << _out_block->Data[1] << _out_block->Data[2] << _out_block->Data[3] << \
                                         _out_block->Data[4] << _out_block->Data[5] << _out_block->Data[6] << _out_block->Data[7] << \
                                         _out_block->Data[8] << _out_block->Data[9];
        //
        default:
        qWarning() << "LanMessage:"<< hex << _out_block->Id << _out_block->Source <<  _out_block->Size << "0" << \
                                         _out_block->MessageCount << _out_block->NotUse << \
                                         _out_block->Data[0] << _out_block->Data[1] << _out_block->Data[2] << _out_block->Data[3] << \
                                         _out_block->Data[4] << _out_block->Data[5] << _out_block->Data[6] << _out_block->Data[7] << \
                                         _out_block->Data[8] << _out_block->Data[9] << "...";
        break;
    }
}


const unsigned int lengthPathEncoderDividerOffPar = 0;

int UMU::xTaskGetTickCount(void)
{
    return GetTickCount_();
}

void UMU::PLDInterruptDisable()
{
    _device->setPLDInterruptEnable(false);
}

void UMU::PLDInterruptEnable()
{
    _device->setPLDInterruptEnable(true);
}

// задержка в единицах по 168 нС
// задавать задержки не более секунды
void UMU::delay(unsigned int value)
{
timespec t;
    t.tv_sec = 0;
    t.tv_nsec = value * 168;
    nanosleep(&t, NULL);
}

void UMU::vSemaphoreCreateBinary(xSemaphoreHandle& pHandle)
{
    pHandle = _device->createCriticalSection();
}

void UMU::TakeSemaphore(xSemaphoreHandle x)
{
    _device->criticalSectionEnter(x);
}

void UMU::xSemaphoreGive(xSemaphoreHandle x)
{
    _device->criticalSectionRelease(x);
}

void UMU::vTaskDelay(unsigned int value)
{
    SLEEP(value);
}

// измерение напряжения аккумулятора
DWORD UMU::get_msrd(void){ return 0;}

void UMU::putmsg(UCHAR *srcbuf, USHORT size)
{
    _device->NonSyncAddToOutBufferLock();
    attachMessageNumber();
    _device->NonSyncAddToOutBuffer(reinterpret_cast<tLAN_CDUMessage*>(srcbuf));
    _device->NonSyncAddToOutBufferUnlock();
}

void UMU::releaseBuffer()
{
unsigned char *destPtr;
unsigned char *srcPtr;
    destPtr = reinterpret_cast<unsigned char*>(&_BScanMessage);
    srcPtr = reinterpret_cast<unsigned char*>(_BScanMessageHeader);
    for (unsigned int ii=0; ii < sizeof(_BScanMessageHeader); ++ii)
    {
        *destPtr++ = *srcPtr++;
    }
    _device->NonSyncAddToOutBuffer(&_BScanMessage);
    _device->NonSyncAddToOutBufferUnlock();
}

void UMU::get_Access(unsigned short size)
{
    DEFCORE_ASSERT(size >= LAN_MESSAGE_BIG_HEADER_SIZE);
    _device->NonSyncAddToOutBufferLock();
    _BScanMessage.Size = size - LAN_MESSAGE_BIG_HEADER_SIZE;
    _BScanMessageCounter = 0;
}


void UMU::put_DataByWord(unsigned int address, USHORT size)
{
    while((size) && (_BScanMessageCounter < _BScanMessage.Size))
    {
        _BScanMessage.Data[_BScanMessageCounter++] = _device->readPLDRegister(usRight, address);
        _BScanMessage.Data[_BScanMessageCounter++] = _device->readPLDRegister(usLeft, address);
        size -= 2;
        address += 2;
    }
}
// загрузка начиная с начала сообщения
void UMU::put_DataByWord(unsigned short *srcBuf, USHORT size)
{
unsigned char *destPtr;
    DEFCORE_ASSERT(!(size & 0x01) && size); // size - четное число (размер в байтах), неравное 0
    destPtr = reinterpret_cast<unsigned char*>(&_BScanMessage);
    for(unsigned short ii=0; ii < size; ii += 2)
    {
        *destPtr++ = (unsigned char)(*srcBuf & 0xFF);
        *destPtr++ = (unsigned char)(*srcBuf >> 8);
        srcBuf++;
        if (ii >= LAN_MESSAGE_BIG_HEADER_SIZE)
        {
            _BScanMessageCounter += 2;
        }
    }
}

unsigned short UMU::Rd_RegPLD(unsigned int regAddr)
{
    return (_device->readPLDRegister(usLeft, regAddr) << 8) | _device->readPLDRegister(usRight, regAddr);
}

void UMU::Wr_RegPLD(unsigned int regAddr, unsigned short value)
{
    _device->writePLDRegister(usLeft, regAddr, (unsigned char)(value >> 8));
    _device->writePLDRegister(usRight, regAddr, (unsigned char)(value & 0xFF));
}

void UMU::writeIntoRAM(unsigned short address, unsigned short value)
{
    _device->writeIntoRAM(usLeft, address, (unsigned char)(value >> 8));
    _device->writeIntoRAM(usRight, address, (unsigned char)(value & 0xFF));
}

unsigned short UMU::readFromRAM(unsigned short address)
{
    return (_device->readFromRAM(usLeft, address) << 8) | _device->readFromRAM(usRight, address);
}

void UMU::ush_init(void)
{
USHORT *r;
DWORD i;

  vSemaphoreCreateBinary(s_ascan);
  vSemaphoreCreateBinary(s_asd);
  vSemaphoreCreateBinary(s_ImitDP);
#ifndef AC_dis
  vSemaphoreCreateBinary(s_ACTH);
#endif
  varsinit();
//
#ifndef DEVICE_EMULATION
//
#ifndef LEVEL_SENSITIVE_PLD_INT_SERVICE
    EXTMODE = 8; // edge sensitive EINT3
#endif

#ifndef DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
    installirq1( TIMER1_INT, (void *)vPortASDEntry, TIMER1_INT_PRIO,FALSE);
    installirq1( EINT3_INT, (void *)intPLD, EINT2_INT_PRIO,FALSE);
#else
    installirq1( EINT3_INT, (void *)vPortASDEntry, EINT2_INT_PRIO,FALSE);
#endif

    PINSEL4 &= ~(3<<26);
    PINSEL4 |= (1<<26); // EINT3 - P2.13
#endif
//
#ifndef DEVICE_EMULATION
   Wr_RegPLD(a0x1300,0x101);

  for (i=ExtRamStartAdr;i<ramstart+(0xD000<<1);i+=2)
  {
    r=(USHORT*)(i);
    *r= 0;
  }
   Wr_RegPLD(a0x1300,0);
//
#ifdef CONFIGURATION_PRINT
    simplePrintf("\n Hardware DP procession");
#endif
//
#ifndef DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
 PCONP |= (1<<2);   // PCTIM1
 T1MR0 = t1MinimumMatch;
 T1MCR = 7;  // reset & stop
#endif
//
  setRelayCtrlSignals(KSignalsOff);

#ifdef LARGE_BSCAN
  if (xTaskCreate(moveLargeBScan, "mvBScan", configMINIMAL_STACK_SIZE,NULL,TASK_MAX_PRIORITY,&h_moveLargeBScan) == false)
            simplePrintf("\nush_init: error - moveLargeBScan task was not created ");
#endif
#endif //DEVICE_EMULATION
}

void UMU::whenStartPLD()
{
    if ((EnableBScan)||(EnableASD))
    {
        intdisf();
        COND_INT_ASD_ENABLE_UNLOCK
        enintpld;
        intenf();
        START_BSCAN_PERIOD
    }\
    TakeSemaphore(s_ascan);
    if (isAScanStopped(TRUE)) flWasStopped=0;
    xSemaphoreGive(s_ascan);
    Wr_RegPLD(a0x1300, 0x202);
}

// line = 0/1  (liniya1/liniya2)
void UMU::read_ascan(UCHAR line)
{
USHORT a,a1,i;
#ifndef DEVICE_EMULATION
USHORT *AScanAddr;
#else
unsigned int AScanAddr;
#endif
register USHORT k; // USHORT - !!!
BOOL frep;

//     smprintf("read_ascan: line = %d",line+1);

#ifdef BUG_SEARCH
   if (ascan[line] == 0) && (ascan[line+2] == 0)
   {
//     smprintf("read_ascan: false call line = %d",line+1);
     return 0;
   }
#endif
//
   TakeSemaphore(s_ascan);
#ifndef DEVICE_EMULATION
   if (!line) AScanAddr = (USHORT*)Aaddr1; // liniya 1
         else  AScanAddr = (USHORT*)Aaddr2;  // liniya 2
#else
   if (!line) AScanAddr = Aaddr1;
         else  AScanAddr = Aaddr2;
#endif
//
  do
  {
   frep = FALSE;
   switch (ascan[line] | ascan[line+2])
   {
   case 1:
   {  // even if one of the ascan[]`s pair equals 0
       if (  (ascan[line] & ascanregim[line] & 0x80 ) || (ascan[line+2] & ascanregim[line+2] & 0x80 ) || (get_tickdur(AscanSendTick[line]) > TimeToTick(AscanSendTime) ) )
       { // ("hotya by odna iz razvertok odnokratnaya") or ("pauza istekla dly ciclicheskoi")
//         smprintf("\n ascan starting - line %d",line);
         a=ascanscale[line] | ascanscale[line+2] << 8;

//         *(USHORT*)((DWORD)AScanAddr+(0xFE<<1)) = a;
          Wr_RegPLD(AScanAddr+(0xFE<<1), a);

         a=ascanstart[line] | ascanstart[line+2] << 8;
//         *(USHORT*)((DWORD)AScanAddr+(0xFD<<1)) = a;
          Wr_RegPLD(AScanAddr+(0xFD<<1), a);
//
         a=curchan[line] | curchan[line+2] << 8;
         if (line) a |= 0x8080;
//         *(USHORT*)((DWORD)AScanAddr+(0xFC<<1))=a;
          Wr_RegPLD(AScanAddr+(0xFC<<1), a);
//
         a=(1+(ascanstrobformax[line]<<2) ) | ( (1+ (ascanstrobformax[line+2]<<2)) << 8);
         a1=((ascanregim[line] & 0xC) << 3) | (((ascanregim[line+2] & 0xC) << 3) << 8) ;  //
         a1=a1 | a;
//         *(USHORT*)((DWORD)AScanAddr+(0xFF<<1)) = a1;
          Wr_RegPLD(AScanAddr+(0xFF<<1), a1);

//   mozhno li ne zapuskat A-razv esli sootvetstv. ascan[] == 0
//
         if (ascan[line]) ascan[line] = 2;
         if (ascan[line+2]) ascan[line+2] = 2;
//
         AscanSendTick[line] = xTaskGetTickCount();
       }
       break;
   }
//
   case 0x2:
   case 0x3:
   case 0x6:
   {
//
       Set_CSPLD;
//       i = *(USHORT*)((DWORD)AScanAddr+(0xFF<<1));
       i = Rd_RegPLD(AScanAddr+ (0xFF<<1));
       Reset_CSPLD;
//
       if ((i & 0x101) == 0)
       {
         if (ascan[line] == 2)  ascan[line] = 4;
         if (ascan[line+2] == 2) ascan[line+2] = 4;
//         frep = TRUE;  go further
//         break;
       }
        else
        {
         if (flWasStopped != 0)
         { // если останавливали работу ПЛИС
             restartAScan();
             flWasStopped = 0;
             break;
         }

         if (get_tickdur(AscanSendTick[line]) > (TimeToTick(AscanSendTime) >> 2))
         {
           if ((ascan[line] == 2) && ((i & 0x1) == 0))
           { // L-ascan  ready and wanted
            if (!(ascanregim[line] & 0x10))
            {
//              smprintf("\n L-ascan line %d is ready",line);
              fill0x7F_hdr(&tempbuf[0])
              for (k = 0; k< 232; ++k)
              {

#ifdef ADC_CONST
                tempbuf[asbdy_offs+k]  = (ADC_CONST_VALUE & 0xFF);
#else
                Set_CSPLD;
//                tempbuf[asbdy_offs+k] = *AScanAddr & 0xFF;
                tempbuf[asbdy_offs+k] = Rd_RegPLD(AScanAddr) & 0xFF;
                Reset_CSPLD;
//                AScanAddr++;
                AScanAddr +=2;
#endif
              }
              tempbuf[slt_offs] = curchan[line] | (line << 6);
              putmsg(tempbuf, szArazv+hdrsize);
            }
//
#ifndef msg0x82_dis
              fill0x82_hdr(&tempbuf[0])
#ifndef DEVICE_EMULATION
              if (!line) AScanAddr = (USHORT*)(Aaddr1 + (0xE8 << 1)); // liniya 1
                     else  AScanAddr = (USHORT*)(Aaddr2 + (0xE8 << 1));  // liniya 2
#else
              if (!line) AScanAddr = Aaddr1 + (0xE8 << 1);
                  else  AScanAddr = Aaddr2 + (0xE8 << 1);
#endif
              tempbuf[slt_offs] = curchan[line] | (line<<6);
#ifdef OLD_PACKET_HEADER
              for (k = 5; k< 8; ++k)
#else
              for (k = 7; k< 10; ++k)
#endif
              {
                 Set_CSPLD;
//                 tempbuf[k] = (UCHAR)*AScanAddr;
                 tempbuf[k] = Rd_RegPLD(AScanAddr) & 0xFF;
                 Reset_CSPLD;
//                AScanAddr++;
                 AScanAddr += 2;
              }
              putmsg(tempbuf,szAAZ+hdrsize);
#endif
//
              ascan[line] = 8;
              frep = TRUE;
              break;
           }
           if ((ascan[line+2] == 2) && ((i & 0x100) == 0))
           {  // R-ascan  ready and wanted
            if (!(ascanregim[line+2] & 0x10))
            {
//              smprintf("\n R-ascan line %d is ready",line);
              fill0x7F_hdr(&tempbuf[(tempbuf_size>>1)])
              for (k = 0; k< 232; ++k)
              {

#ifdef ADC_CONST
              tempbuf[(tempbuf_size>>1)+asbdy_offs+k]  =  ADC_CONST_VALUE >> 8;
#else

                Set_CSPLD;
//                tempbuf[(tempbuf_size>>1)+asbdy_offs+k] = *AScanAddr >> 8;
                tempbuf[(tempbuf_size>>1)+asbdy_offs+k] = Rd_RegPLD(AScanAddr) >> 8;
                Reset_CSPLD;
#endif
//                AScanAddr++;
                AScanAddr += 2;
              }
              tempbuf[(tempbuf_size>>1)+slt_offs] = curchan[line+2] | (line<<6) | 0x80;  // right hand side

              putmsg(&tempbuf[tempbuf_size>>1],szArazv+hdrsize);
            }
#ifndef msg0x82_dis
             fill0x82_hdr(&tempbuf[(tempbuf_size>>1)])
#ifndef DEVICE_EMULATION
              if (!line) AScanAddr = (USHORT*)(Aaddr1 + (0xE8 << 1)); // liniya 1
                     else  AScanAddr = (USHORT*)(Aaddr2+ (0xE8 << 1));  // liniya 2
#else
              if (!line) AScanAddr = Aaddr1 + (0xE8 << 1);
                  else  AScanAddr = Aaddr2+ (0xE8 << 1);
#endif

              tempbuf[(tempbuf_size>>1)+slt_offs] = curchan[line+2] | (line<<6) | 0x80;  // right hand side
#ifdef OLD_PACKET_HEADER
              for (k = 5; k< 8; ++k)
#else
              for (k = 7; k< 10; ++k)
#endif
              {
                 Set_CSPLD;
//                 tempbuf[k+(tempbuf_size>>1)] = *AScanAddr >> 8;
                 tempbuf[k+(tempbuf_size>>1)] = Rd_RegPLD(AScanAddr) >> 8;
                 Reset_CSPLD;
//                 AScanAddr++;
                 AScanAddr +=2;
              }
              putmsg(&tempbuf[tempbuf_size>>1],szAAZ+hdrsize);
#endif
//
             ascan[line+2] = 8;
             frep = TRUE;
             break;
           }
// error message if not ready
           simplePrintf("\nread_ascan: line %d - Ascan is NOT ready",line);
           restartAScan();
         }
         break;
        } // else
   }
//
// сюда ничего не вставлять - должны провалиться из предыдущего case
//
   case 0x4:
   { // both sides are ready
//   smprintf("\nascan: line %d  2 side scan is  ready",line);
      fill0x7F_hdr(&tempbuf[0])
      fill0x7F_hdr(&tempbuf[(tempbuf_size>>1)])
      for (k = 0; k< 232; ++k)
      {
         Set_CSPLD;
#ifdef ADC_CONST
         i = (ADC_CONST_VALUE & 0xFF) | ADC_CONST_VALUE << 8;
#else
//         i = *AScanAddr;
         i = Rd_RegPLD(AScanAddr);
#endif

         Reset_CSPLD;
         tempbuf[asbdy_offs+k] = i & 0xFF;  // Left ascan
         tempbuf[(tempbuf_size>>1)+asbdy_offs+k] = i >> 8;  // Right ascan
//         AScanAddr++;
          AScanAddr +=2;
      }

      if (ascan[line])
      {
        if (!(ascanregim[line] & 0x10))
        {
           tempbuf[slt_offs] = curchan[line] | (line<<6) ;  // left hand side
           putmsg(tempbuf,szArazv+hdrsize);
        }
        ascan[line] = 8;
      }
      if (ascan[line+2])
      {
       if (!(ascanregim[line+2] & 0x10))
       {
           tempbuf[(tempbuf_size>>1)+slt_offs] = curchan[line+2] | (line<<6) | 0x80;  // right hand side
           putmsg(&tempbuf[tempbuf_size>>1],szArazv+hdrsize);
       }
       ascan[line+2] = 8;
      }
//
#ifndef msg0x82_dis
      if (ascan[line] | ascan[line+2])
      {
        fill0x82_hdr(&tempbuf[0])
        fill0x82_hdr(&tempbuf[(tempbuf_size>>1)])
#ifndef DEVICE_EMULATION
        if (!line) AScanAddr = (USHORT*)(Aaddr1 + (0xE8 << 1)); // liniya 1
             else  AScanAddr = (USHORT*)(Aaddr2 + (0xE8 << 1));  // liniya 2
#else
        if (!line) AScanAddr = Aaddr1 + (0xE8 << 1);
            else  AScanAddr = Aaddr2 + (0xE8 << 1);
#endif
        tempbuf[slt_offs] = curchan[line] | (line<<6) ;  // left hand side
        tempbuf[(tempbuf_size>>1)+slt_offs] = curchan[line+2] | (line<<6) | 0x80;  // right hand side

#ifdef OLD_PACKET_HEADER
              for (k = 5; k< 8; ++k)
#else
              for (k = 7; k< 10; ++k)
#endif
              {
                 Set_CSPLD;
//                 a = *AScanAddr++;
                 a = Rd_RegPLD(AScanAddr);
                 AScanAddr +=2;
                 Reset_CSPLD;
                 tempbuf[k] = (UCHAR)a;
                 tempbuf[k+(tempbuf_size>>1)] = a >> 8;
              }
              if (ascan[line]) putmsg(&tempbuf[0],szAAZ+hdrsize);
              if (ascan[line+2]) putmsg(&tempbuf[tempbuf_size>>1],szAAZ+hdrsize);
      }
#endif
// go further
//      frep = TRUE;
//      break;
   }
//
// сюда ничего не вставлять - должны провалиться из предыдущего case
   case 0x8:
   case 0x9:
   case 0xA:
   {
      for (i=0; i<2; ++i)
      {
        k = line+(i<<1);
        if (ascan[k] > 1)
        {
           if ((ascanregim[k] & 0x80) == 0)
           {
              if (ascanregim[k] & 0x40)
              {
                 curchan[k]++;
                 if (curchan[k]==NumOfTacts) curchan[k] = 0;
              }
              ascan[k] = 1;
           }
             else ascan[k] = 0; // odnokr.regim
        }
      }
      break;
   }
   }
   } while (frep);
//
   xSemaphoreGive(s_ascan);
}
//-------------------------------------------------------------------
// д®а¬ЁагҐв ®вўҐв б ­ Їап¦Ґ­ЁҐ¬  ЄЄг¬г«пв®а
// UЁ§¬/Uakk = 332/3632 - б¬.¤Ґ«ЁвҐ«м
// ®Ї®а­®Ґ ­ ЇаҐ¦Ґ­ЁҐ Ђ–Џ ЇаҐ¤Ї®« Ј Ґвбп а ў­л¬ 3,3‚,
// Ґ¬г б®®вўҐвбвўгҐв зЁб«® 1023
void UMU::Get_Uakk(void)
{
DWORD u = get_msrd() *1198560 / 3396360; // ў ¤Ґбпвле ¤®«пе ў®«мв
  O1Data[0] = idVolt;
  O1Data[1] = idBUM;
  O1Data[2]  = szVolt;
  O1Data[3]  = 0;
//
  O1Data[hdrsize]  = u/10;
  O1Data[hdrsize+1]  = u%10;

  O1Data[5]  = 0;
  putmsg(O1Data,szVolt+hdrsize);
}
//-------------------------------------------------------------------
//!
void UMU::SetNumOfTacts(UCHAR *ptr)
{
register USHORT a;

  smprintf("\nSetNumOfTacts enter");

  Stop_PLD_Work();

  TBD

#ifndef LARGE_BSCAN
  StartBScanFIFO=EndBScanFIFO;
#endif

  NumOfTacts=*ptr;
  a=NumOfTacts-1;
  a = a | (a<<8);
  Wr_RegPLD(a0x1301, a);
  Start_PLD_Work();
}
//-------------------------------------------------------------------
BOOL UMU::isAScanStopped(BOOL fUnlocked)
{
register BOOL res = TRUE;
register unsigned char ii;


  if (fUnlocked == FALSE) TakeSemaphore(s_ascan);
  for (ii=0; ii<4; ++ii)
  {
      if (ascan[ii] != 0)
      {
          res = FALSE;
          break;
      }
  }
  if (fUnlocked == FALSE) xSemaphoreGive(s_ascan);
  return res;
}

//-------------------------------------------------------------------
void UMU::StopAScan(void)
{
register UCHAR ii;

  TakeSemaphore(s_ascan);
  for (ii=0; ii<4; ++ii) ascan[ii] = 0;
//  jj = ascancounter;
  xSemaphoreGive(s_ascan);
//
/*
  O1Data[0] = 0x46;
  O1Data[1] = idBUM;
  O1Data[2]  = 4;
  O1Data[3]  = 0;
#ifndef OLD_PACKET_HEADER
   O1Data[5] = 0;
#endif
//
  WriteLE32U(&O1Data[hdrsize],jj);
  putmsg(O1Data,4+hdrsize);
//
  simplePrintf("\n %d ascan were sent",jj);
*/
}
//-------------------------------------------------------------------
void UMU::StartBScan(void)
{
  smprintf("\nStartBScan enter");

  if ((mainShiftSensorNumber & IMITATORMASK) == 0)
  { // так как основной ДП могли покрутить, достаточно 2х чтений, но делаем 3
// подобно сделано в doWhenImitSwitchOFF(), но здесь п.п с проверками на номер ДП
  register unsigned char ii;
      for(ii=0; ii<3; ++ii)
      {
          getShiftSensorValue(mainShiftSensorNumber);
          delay(10);
      }
      mainShiftSensorPathcoordMemorized = Pathcoord[mainShiftSensorNumber];
      speedTimer = xTaskGetTickCount();
  }

#ifdef LARGE_BSCAN
  initMScanFlags();
  Wr_RegPLD(ACControlReg, ACControlValue_EVAL);
  Wr_RegPLD(ACSumStartReg, ACSumStartValue);
#else
    fMScan = 0;
    fMScan1 = 0;
   MScanSendTick = xTaskGetTickCount();
#endif
  EnableBScan=1;
  Start_PLD_Work();
}
//-------------------------------------------------------------------
void UMU::StopBScan(void)
{
  smprintf("\nStopBScan enter");
  EnableBScan=0;
  ASD_Off();
#ifndef AC_dis
  acMessageOff();
#endif
  Stop_PLD_Work();
//
// Ї®¤вўҐа¦¤Ґ­ЁҐ ўлЇ®«­Ґ­Ёп Є®¬ ­¤л
  O1Data[0] = 0x48;
  O1Data[1] = idBUM;
  O1Data[2]  = 0;
  O1Data[3]  = 0;
//
  O1Data[5] = 0;
  putmsg(O1Data,hdrsize);
}
//
//-------------------------------------------------------------------
//!
void UMU::Stop_PLD_Work(void)
{
//  smprintf("\nStop_PLD_Work enter");
    TakeSemaphore(s_ascan);
    Wr_RegPLD(a0x1300, 0); // сигнал прерывания станет неактивным
    flWasStopped=1;
    xSemaphoreGive(s_ascan);

    intdisf(); // чтобы исключить переключение задач

    BSCAN_READY_SIGN_CLEAR

#ifndef DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
    disintT1
    RESET_INT_T1
#endif
    disintpld
    RESET_INT_PLD
    COND_INT_ASD_ENABLE_LOCK
    intenf();
}
//-------------------------------------------------------------------
void UMU::Start_PLD_Work(void)
{
//    smprintf("\nStart_PLD_Work enter");
    Wr_RegPLD(a0x1300, 0); // PLD registers may not be accessble
// пока прерывания от ПЛИС невозможны
#ifndef AC_dis
    TakeSemaphore(s_ACTH);
    if (ACCalcStage == ACCALC_WAITFORDATA)
    {
        ACCalcStage = ACCALC_NEED_TO_START;
    }
        else if (ACCalcStage == ACTHCALC_WAITFORDATA)
             {
                 ACCalcStage = ACTHCALC_CNTQUERY;
             }
    xSemaphoreGive(s_ACTH);
#endif
    WHEN_START_PLD
}
//
//-------------------------------------------------------------------
void UMU::StartPLD(void)
{
//    smprintf("\nStartPLD enter");
    Wr_RegPLD(a0x1300, 0); // PLD registers may not be accessble
// пока прерывания от ПЛИС невозможны
    WHEN_START_PLD
}
//
//-------------------------------------------------------------------
unsigned char UMU::correctDACValue(unsigned char value)
{
if (value < c_dacminval) value = c_dacminval;
    else if (value > c_dacmaxval) value = c_dacmaxval;
    return  value + c_dacCorrection;
}
//-------------------------------------------------------------------
// очиститка области параметров такта не производится, т.к.
// п.п может вызываться, когда B-развертка включена, а здесь время
// в призме не устанавливается
void UMU::SetTacktParam(tTACTPARAMSCMD *pData)
{
#ifndef DEVICE_EMULATION
unsigned short *BA;
#else
unsigned short BA;
#endif
unsigned short a;
unsigned short b;
unsigned short ii;
unsigned char takt;
DWORD jj;
unsigned char *p;

  takt = pData->TacktNumber;  // takt number 0...

  smprintf("\nSetTaktParam enter: takt = %d",takt);

  if (takt >= (tactbitmsk+1))
  {
      simplePrintf("\nSetTaktParam: takt number is wrong - %d", takt);
      return;
  }

  Stop_PLD_Work();
  Wr_RegPLD(a0x1300, 0x101);

/*
  simplePrintf("\nSetTaktParam: pData->ACATTLn1L - %d", pData->ACATTLn1L);
  simplePrintf("\nSetTaktParam: pData->ACATTLn1R - %d", pData->ACATTLn1R);
  simplePrintf("\nSetTaktParam: pData->ACATTLn2L - %d", pData->ACATTLn2L);
  simplePrintf("\nSetTaktParam: pData->ACATTLn2R - %d", pData->ACATTLn2R);
*/

  if (pData->Duration > MaxAScanDuration) pData->Duration = MaxAScanDuration; // ограничить длительность развертки

  jj = parreg_sz * takt + ExtRamStartAdr;
//
  if (!takt)
  {
#ifndef DEVICE_EMULATION
     BA=(USHORT*)(jj + _WSALmask); // offset WSALmask
     *BA++ = 0;
#else
      BA = jj + _WSALmask;
      writeIntoRAM(BA, 0);
      BA += 2;
#endif
     a = extramstart + parreg_sz * NumOfTacts; // low byte = 0 here

     smprintf("\nSetTacktParam: takt 0, WSA = %x", a);

     a = a | (a >> 8);
#ifndef DEVICE_EMULATION
     *BA=a;
#else
     writeIntoRAM(BA, a);
#endif

//очистить рабочую область такта, считая ее максимально длинной, если
// B-развертка не включена
    if (EnableBScan == 0)
    {
#ifndef DEVICE_EMULATION
        BA = (USHORT*)( (a & 0xFF00) + ramstart) ;
        for(b=0; b<256*4; ++b) BA[b] = 0;
        for(b=0; b < AContactZone; ++b)
        {
            BA[((b + pData->Duration) << 2) + 1] = (correctDACValue(pData->ACATTLn1L) << 8) | correctDACValue(pData->ACATTLn1R);
            BA[((b + pData->Duration) << 2) + 2] = (correctDACValue(pData->ACATTLn2L) << 8) | correctDACValue(pData->ACATTLn2R);
        }
/*
#else
// если DEVICE_EMULATION  - рабочую область тактов здесь не обнуляем

        BA = a & 0xFF00;
        for(b=0; b<256*4; ++b)
        {
            writeIntoRAM(BA + b, 0);
        }
        for(b=0; b < AContactZone; ++b)
        {
            writeIntoRAM(BA + ((b + pData->Duration) << 2) + 1, (correctDACValue(pData->ACATTLn1L) << 8) | correctDACValue(pData->ACATTLn1R));
            writeIntoRAM(BA + ((b + pData->Duration) << 2) + 2, (correctDACValue(pData->ACATTLn2L) << 8) | correctDACValue(pData->ACATTLn2R));
        }
*/
#endif
    }
//     smprintf("\nSetTacktParam: takt - wraddr = %x", BA);
  }
    else
    {
#ifndef DEVICE_EMULATION
         BA = (USHORT*)(parreg_sz * (takt-1) + ExtRamStartAdr + _WSALmask); // offset WSALmask v parametrah predydushego takta
         b = *BA++;
         a = *BA;
#else
         BA = extramstart + parreg_sz * (takt-1) + _WSALmask;
         b = readFromRAM(BA);
         BA += 2;
         a = readFromRAM(BA);
#endif
          if ( (b == 0) && (a != 0) )
          {
              a <<= 8;
#ifndef DEVICE_EMULATION
              BA=(USHORT*)(parreg_sz * (takt-1) + ExtRamStartAdr + _RazvCRmask);
              b = *BA + AContactZone; // не должно превысить 255
#else
              BA = (extramstart + parreg_sz * (takt-1) + _RazvCRmask);
              b = readFromRAM(BA) + AContactZone; // не должно превысить 255
#endif
              a += ((b & 0xFF) * 4 + 100) << 1;  // "<<1", i.e word addressing
              if (a & 0xFF) a += 0x100; // ceiling
              a &= 0xFF00;

              smprintf("\nSetTacktParam: takt %d, WSA = %x", takt,a);

              a = a | (a>>8);
#ifndef DEVICE_EMULATION
              BA=(USHORT*)(jj + _WSALmask); // offset WSALmask
              *BA++ = 0;
              *BA = a;
#else
              BA = jj + _WSALmask;
              writeIntoRAM(BA, 0);
              BA += 2;
              writeIntoRAM(BA, a);
#endif
//
//очистить рабочую область такта, считая ее максимально длинной, если
// B-развертка не включена
             if (EnableBScan == 0)
             {
#ifndef DEVICE_EMULATION
                 BA = (USHORT*)( (a & 0xFF00) + ramstart) ;
                 for(b=0; b<256*4; ++b) BA[b] = 0;
                 for(b=0; b < AContactZone; ++b)
                 {
                     BA[((b + pData->Duration) << 2) + 1] = (correctDACValue(pData->ACATTLn1L) << 8) | correctDACValue(pData->ACATTLn1R);
                     BA[((b + pData->Duration) << 2) + 2] = (correctDACValue(pData->ACATTLn2L) << 8) | correctDACValue(pData->ACATTLn2R);
                 }
/*
#else
// если DEVICE_EMULATION  - рабочую область тактов здесь не обнуляем

                 BA = a & 0xFF00;
                 for(b=0; b<256*4; ++b)
                 {
                     writeIntoRAM(BA + b, 0);
                 }
                 for(b=0; b < AContactZone; ++b)
                 {
                     writeIntoRAM(BA + ((b + pData->Duration) << 2) + 1, (correctDACValue(pData->ACATTLn1L) << 8) | correctDACValue(pData->ACATTLn1R));
                     writeIntoRAM(BA + ((b + pData->Duration) << 2) + 2, (correctDACValue(pData->ACATTLn2L) << 8) | correctDACValue(pData->ACATTLn2R));
                 }
*/
#endif
             }
         }
            else
            { // WSA takta ii-1 ne opredelen
              simplePrintf("\nSetTacktParam: takt's %d WSA not defined and HiByte = %x, LoByte = %x",takt-1, a,b);
            }
    }
//
#ifndef DEVICE_EMULATION
  BA=(USHORT*)(jj + _ChLCRmask); // offset ChLCRmask

#ifndef AC_dis
  if ((pData->GenRcvLn1L & 0x0F) < NumberOfReceivers)
  {
          receiversInCycle[indexLeft][0][takt] = pData->GenRcvLn1L & 0x0F;
  }
      else
      {
          receiversInCycle[indexLeft][0][takt] = RECEIVER_UNKNOWN;
      }
  if ((pData->GenRcvLn1R & 0x0F) < NumberOfReceivers)
  {
          receiversInCycle[indexRight][0][takt] = pData->GenRcvLn1R & 0x0F;
  }
      else
      {
          receiversInCycle[indexRight][0][takt] = RECEIVER_UNKNOWN;
      }
#endif
  a = pData->GenRcvLn1R | (pData->GenRcvLn1L << 8);  // liniya 1
//
//  smprintf("\nSetTacktParam: line 1, genr-rcvr - genl-rcvl = %x", a);
//
  *BA++=a;
//
#ifndef AC_dis
  if ((pData->GenRcvLn2L & 0x0F) < NumberOfReceivers)
  {
          receiversInCycle[indexLeft][1][takt] = pData->GenRcvLn2L & 0x0F;
  }
      else
      {
          receiversInCycle[indexLeft][1][takt] = RECEIVER_UNKNOWN;
      }
  if ((pData->GenRcvLn2R & 0x0F) < NumberOfReceivers)
  {
          receiversInCycle[indexRight][1][takt] = pData->GenRcvLn2R & 0x0F;
  }
      else
      {
          receiversInCycle[indexRight][1][takt] = RECEIVER_UNKNOWN;
      }
#endif
  a = pData->GenRcvLn2R | (pData->GenRcvLn2L << 8);  // liniya 2
//
//  smprintf("\nSetTacktParam: line 2, genr-rcvr - genl-rcvl = %x", a);
//
  *BA=a;
#endif
//
  a =  pData->Duration;
  a =  a | (a << 8);
#ifndef DEVICE_EMULATION
  BA=(USHORT*)(jj + _RazvCRmask); // offset RazvCRmask
  *BA=a;
#else
  BA = jj + _RazvCRmask;
  writeIntoRAM(BA, a);
#endif
  smprintf("\nSetTacktParam: AScan length = 0x%x", a);
//
#ifndef DEVICE_EMULATION
// з бв®в  ‡€ «Ё­Ёп 1,2
  {
    BA=(USHORT*)(jj + _FreqL1mask);
    *BA = ((pData->PulseFreqL & 1) << 8)  |  (pData->PulseFreqR & 1);   // skrestit lev i prav storony
    smprintf("\nSetTacktParam: line's 1  Freq value = 0x%x",*BA);
    BA=(USHORT*)(jj + _FreqL2mask);
    *BA = ((pData->PulseFreqL & 0x10) << 4)  |  ((pData->PulseFreqR & 0x10) >> 4);   //
    smprintf("\nSetTacktParam: line's 2  Freq value = 0x%x",*BA);
  }
//
// uroven stroba 0 levoi storoni liniya 1 ... stroba 3 pravoi sotony liniya 2
  BA=(USHORT*)(jj + _LevStrL0mask);
  for (ii=0,p = (unsigned char*)&pData->Gate0LevelLn1R; ii<8; ++ii)
  {
    a = *p | (*(p+8) << 8); // skrestit lev i prav storony

#ifdef _us46_cpp_prn
    if (ii < 4)   smprintf("\nSetTacktParam: line 1, gate %d, levelr - levell  = %x", ii,a);
         else  smprintf("\nSetTacktParam: line 2, gate %d, levelr - levell  = %x", ii-4,a);
#endif

    *BA++=a;    //
    p++;
  }
//
// amplitudy zi
    BA=(USHORT*)(jj + _AMPZI1mask);
    for (ii=0,p = (unsigned char*)&pData->PulseAmpLn1R ; ii<2; ++ii)
    {
     a = ((15 - ((*p & 7) << 1))*10) | ((15 - ((*(p+2) & 7) << 1))*10 << 8); // skrestit lev i prav storony
     *BA++ = a;    //
     p++;
    }
//
// Изменяем только биты BScanSignalsCutOnParamBitNum в соответствии с командой
    BA=(USHORT*)(jj + BScanCutStartFrac1mask);
    a = *BA;
    a &=  ~((1 << BScanSignalsCutOnParamBitNum) | (1 << (BScanSignalsCutOnParamBitNum+8)));
    a |= ((pData->SwitcherLn1R & BscanSignalsCutOnMask != 0) << BScanSignalsCutOnParamBitNum) | ((pData->SwitcherLn1L & BscanSignalsCutOnMask != 0) << (BScanSignalsCutOnParamBitNum+8));
    *BA = a;
    BA=(USHORT*)(jj + BScanCutStartFrac2mask);
    a = *BA;
    a &=  ~((1 << BScanSignalsCutOnParamBitNum) | (1 << (BScanSignalsCutOnParamBitNum+8)));
    a |= ((pData->SwitcherLn2R & BscanSignalsCutOnMask != 0) << BScanSignalsCutOnParamBitNum) | ((pData->SwitcherLn2L & BscanSignalsCutOnMask != 0) << (BScanSignalsCutOnParamBitNum+8));
    *BA = a;
//
   TakeSemaphore(s_asd); // precaution: if ASD is not off
   for (ii = 0; ii < tactbitmsk+1; ++ii)
   {
     for (jj = 0; jj < 5; ++jj) ASD_PreBuf[ii][jj] = 0xFFFF;
   }
   xSemaphoreGive(s_asd);
//
#endif
  Start_PLD_Work();
}
//-------------------------------------------------------------------
void UMU::ChangeVRU(UCHAR *p)
{
#ifndef DEVICE_EMULATION
USHORT *BA;
#else
USHORT BA;
#endif

USHORT WSA,a;
UCHAR sideidx,lineidx,a1,a2;
uint i,tmp,tmp2, duration;
float quoeff;

  smprintf("\nChangeVRU enter");

  Stop_PLD_Work();
  Wr_RegPLD(a0x1300, 0x101);
//
  sideidx = getSideIndex(p);
  lineidx = getLineIndex(p);
//
#ifndef DEVICE_EMULATION
  BA = (USHORT*)(parreg_sz * (*p & tactbitmsk) + ExtRamStartAdr + _RazvCRmask);
  duration = *BA & 0xFF;
// WSA address is the same for both sides - use left side WSA value
  BA = (USHORT*)(parreg_sz*(*p & tactbitmsk)+ExtRamStartAdr + _WSALmask);
  a = *BA++;
  Lo(WSA)=(UCHAR)a;
  a=*BA;
  Hi(WSA)=(UCHAR)a;
#else
  BA = extramstart + parreg_sz * (*p & tactbitmsk) + _RazvCRmask;
  duration = readFromRAM(BA) & 0xFF;
// WSA address is the same for both sides - use left side WSA value
  BA = extramstart + parreg_sz * (*p & tactbitmsk) + _WSALmask;
  a = readFromRAM(BA);
  Lo(WSA)=(UCHAR)a;
  BA += 2;
  a = readFromRAM(BA);
  Hi(WSA)=(UCHAR)a;
#endif
//  simprintf("\nChangeVRU: WSA = %x",WSA);

  if (WSA<extramstart) WSA= extramstart;
//
  p++;
  tmp = *p++;           // start point delay
  a1 = *p++;            // start point level
  tmp2 = *p++;          // end point delay
  a2 = *p++;            // end point level
//
  a1 =  correctDACValue(a1);
  a2 =  correctDACValue(a2);
//

  if (tmp2 != tmp)  quoeff=(float)(a2-a1)/(tmp2-tmp);
//
  for (i=tmp; i<=tmp2 && i<duration; ++i)
  {
#ifndef DEVICE_EMULATION
      BA = (USHORT*)(WSA+(((i<<2)+lineidx+1)<<1) + ramstart);
      a = *BA;
#else
      BA = WSA + (((i<<2)+lineidx+1)<<1);
      a = readFromRAM(BA);
#endif
      //      simprintf("\nChangeVRU: BA = %x",BA);

      if (sideidx)
      { // income data for the right side
         a &= 0xFF;
         a |= (a1+(uchar)(quoeff*(i-tmp))) << 8;
      }
       else
       {
         a &= 0xFF00;
         a |= (a1+(uchar)(quoeff*(i-tmp)));
       }
#ifndef DEVICE_EMULATION
      *BA = a;
#else
      writeIntoRAM(BA, a);
#endif
  }
  Start_PLD_Work();
//
}
//-------------------------------------------------------------------
//
void UMU::ChangeStrobs(UCHAR *p)
{
#ifndef DEVICE_EMULATION
USHORT *BA;
#else
USHORT BA;
#endif

USHORT WSA,a;
UCHAR sideidx,lineidx,takt,a1,a2;
USHORT i,tmp,tmp2;
USHORT razvLen;
//
  smprintf("\nChangeStrobs enter");
  Stop_PLD_Work();
  Wr_RegPLD(a0x1300, 0x101);
//
  sideidx = getSideIndex(p);
  lineidx =  getLineIndex(p);
  takt = getTacktNumber(p);
  p++;         // *p - nomer stroba 0..
//
#ifndef DEVICE_EMULATION
// WSA address is the same for both sides - use left side WSA value
  BA = (USHORT*)(parreg_sz * takt + ExtRamStartAdr + _WSALmask);
  a = *BA++;
  Lo(WSA)=(UCHAR)a;
  a=*BA;
  Hi(WSA)=(UCHAR)a;
#else
  BA = extramstart + parreg_sz * takt + _WSALmask;
  a = readFromRAM(BA);
  Lo(WSA)=(UCHAR)a;
  BA += 2;
  a = readFromRAM(BA);
  Hi(WSA)=(UCHAR)a;
#endif

  if (WSA<extramstart) WSA= extramstart;
//
#ifndef DEVICE_EMULATION
// uroven stroba
  if (lineidx) BA=(USHORT*)(parreg_sz*takt+ExtRamStartAdr + _LevStrR0mask);
    else  BA= (USHORT*)(parreg_sz*takt+ExtRamStartAdr + _LevStrL0mask);
  for (i=0; i<*p; ++i)  BA++;
  a = *BA;
//
  if (sideidx)
  {
    a &= 0xFF;
    if (*(p+4) == 0) a |= *(p+3) << 8; // 1echo
        else a = a | ((*(p+3) | 1) << 8); // 2echo
  }
    else
    {
      a &= 0xFF00;
      if (*(p+4) == 0) a |= *(p+3);
          else a = a | *(p+3) | 1;
    }
  *BA = a;
//
//   simplePrintf("\nChangeStrobs: takt = %d, strob = %d, addr = 0x%x, level = 0x%x ",takt, *p, (DWORD)BA,a );
#endif

  if (lineidx!=0)
  {
    a1=mask[(*p & 0x03) | 0x04];// else smesch=4
    a2=mask2[(*p & 0x03) | 0x04];
  }
      else
      {
        a1=mask[*p & 0x03];
        a2=mask2[*p & 0x03];
      }
//
  if (sideidx)
  {
   if (*(p+5) & 1) ASDtype[takt] |= (a1 << 8);
      else  ASDtype[takt] &= (a2 << 8) | 0xFF;
  }
    else
    {
       if (*(p+5)&1) ASDtype[takt] |= a1;
             else ASDtype[takt] &= a2 | 0xFF00;
    }

  ASD_Buffer[takt] = ASDtype[takt];

  tmp =  *(p+1);  // strob start
  tmp2 = *(p+2);  // strob end

//      simplePrintf("\nStrob: WSA = %x   ",WSA);
//      simplePrintf("\nStrob:  start = %d   ",tmp);
//      simplePrintf("\nStrob:  end = %d   ",tmp2);
#ifndef DEVICE_EMULATION
     BA = (USHORT*)(parreg_sz * takt + ExtRamStartAdr + _RazvCRmask);
     razvLen = *BA & 0xFF;
#else
     BA = extramstart + parreg_sz * takt + _RazvCRmask;
     razvLen = readFromRAM(BA) & 0xFF;
#endif
      for (i=0; i<razvLen; ++i)
      {
#ifndef DEVICE_EMULATION
        BA=(USHORT*)(WSA + (((i<<2)+3)<<1) + ramstart);
        a = *BA;
#else
          BA= WSA + (((i<<2)+3)<<1);
          a = readFromRAM(BA);
#endif
        if ((i>=tmp) && (i < tmp2))
        {
            if (sideidx)  a |= (a1 << 8);
                else        a |= a1;
        }
         else
         {
           if (sideidx) a &= (a2 << 8) | 0xFF;
              else a &= a2 | 0xFF00;
         }
//
#ifndef DEVICE_EMULATION
        *BA = a;
#else
        writeIntoRAM(BA, a);
#endif
     }
  Start_PLD_Work();
}
//-------------------------------------------------------------------
void UMU::Change2Tp(UCHAR *p)
{
USHORT a, tmp, *BA;
UCHAR sideidx,lineidx;
//
  smprintf("\nChange2TP enter");
//
  Stop_PLD_Work();
  Wr_RegPLD(a0x1300, 0x101);
//
  sideidx = getSideIndex(p);
  lineidx =  getLineIndex(p);
//
  tmp = *(p+1) | (*(p+2) << 8);

  if (tmp > TprizmMax)
  {
     if (tmp < TprizmLargeMax)   tmp=TprizmLargeMax-tmp;
           else tmp = 0;
    Wr_RegPLD(a0x13A0, 0x202);
  }
     else
     {
         tmp = TprizmMax - tmp;
         Wr_RegPLD(a0x13A0, 0);
     }

//  simplePrintf("\n side = %d, line = %d, Tprizm = %d", sideidx, lineidx, tmp);

#ifndef DEVICE_EMULATION
  if (lineidx)
  {
     BA=(USHORT*)(parreg_sz*(*p & tactbitmsk)+ExtRamStartAdr + _TprizmR1mask);
     a = *BA;
     if (sideidx)
     {
        a &= 0xFF;
        a |= tmp << 8;
     }
       else
       {
         a &= 0xFF00;
         a |= tmp & 0xFF;
       }
    *BA = a;
//
     BA=(USHORT*)(parreg_sz*(*p & tactbitmsk)+ExtRamStartAdr + _TprizmR2mask);
     a = *BA;
     if (sideidx)
     {
        a &= 0xFF;
        a |= tmp & 0xFF00;
     }
       else
       {
         a &= 0xFF00;
         a |= tmp >> 8;
       }
  }
    else
    {
       BA=(USHORT*)(parreg_sz*(*p & tactbitmsk)+ExtRamStartAdr + _TprizmL1mask);
       a = *BA;
       if (sideidx)
       {
        a &= 0xFF;
        a |= tmp << 8;
       }
       else
       {
         a &= 0xFF00;
         a |= tmp & 0xFF;
       }
      *BA = a;
//
      BA=(USHORT*)(parreg_sz*(*p & tactbitmsk)+ExtRamStartAdr + _TprizmL2mask);
      a = *BA;
      if (sideidx)
      {
        a &= 0xFF;
        a |= tmp & 0xFF00;
      }
       else
       {
         a &= 0xFF00;
         a |= tmp >> 8;
       }
    }
  *BA = a;
#else
#endif
  Start_PLD_Work();
}
//-------------------------------------------------------------------
void UMU::setBScanSignalsCutStartCmd(tBSCANSIGNALSCUTSTARTCMD * pData)
{
unsigned short a, *BA;
unsigned char sideidx,lineidx,takt;
//
  smprintf("\nsetBScanSignalsCutStartCmd enter");
//
  Stop_PLD_Work();
  Wr_RegPLD(a0x1300, 0x101);
//
  sideidx = getSideIndex(&pData->SLT);
  lineidx =  getLineIndex(&pData->SLT);
  takt = getTacktNumber(&pData->SLT);
//
#ifndef DEVICE_EMULATION
  if (lineidx)
  {
      BA=(USHORT*)(parreg_sz* takt+ExtRamStartAdr + BScanCutStart2mask);
  }
      else
      {
          BA=(USHORT*)(parreg_sz* takt+ExtRamStartAdr + BScanCutStart1mask);
      }
      a = *BA;
      if (sideidx)
      {
        a &= 0xFF;
        a |= pData->Start << 8;

      }
          else
          {
              a &= 0xFF00;
              a |= pData->Start;
          }
   *BA = a;
   if (lineidx)
   {
       BA=(USHORT*)(parreg_sz* takt+ExtRamStartAdr + BScanCutStartFrac2mask);
   }
      else
      {
          BA=(USHORT*)(parreg_sz* takt+ExtRamStartAdr + BScanCutStartFrac1mask);
      }
    a = *BA;
    if (sideidx)
      {
        a &=  0xFF | (1<< (BScanSignalsCutOnParamBitNum+8));
        a |= (pData->StartFrac & BscanSignalsCutStartMask) << 8;
      }
          else
          {
              a &= 0xFF00 | (1<< BScanSignalsCutOnParamBitNum);
              a |= pData->StartFrac & BscanSignalsCutStartMask;
          }
   *BA = a;
#else
#endif
  Start_PLD_Work();
}

void UMU::setExendedFormatOfDPValMessageCmd(BOOL state)
{
    DP_SEMAPHORE_ENTER
    if (state == 0) fDPCoordDataExtended = 0;
        else fDPCoordDataExtended = 1;
    DP_SEMAPHORE_LEAVE
}

void UMU::ReadASD(void)
{
USHORT a, r;
register UCHAR i,j;
USHORT at,ab;
//USHORT *pasd;

   Wr_RegPLD(BScanASDCRegAddr, 0);
//
   r = BScanASD_0;
   for (i=0; i<NumOfTacts; ++i)
   {
     a = Rd_RegPLD(r);
     at=ASDtype[i];

     ab = 0;
     for (j=0; j<4; ++j)
     {
       ab |= ASD_PreBuf[i][j];
       ASD_PreBuf[i][j] =  ASD_PreBuf[i][j+1];
     }
     ab |= a;
     ASD_PreBuf[i][4] = a;
     ASD_Buffer[i] |=(a & ~at);
     ASD_Buffer[i] &=(ab | ~at);
     r += 2;
   }
}
//-------------------------------------------------------------------
// uses AScan buffer - tempbuf
void UMU::read_asd(void)
{
register UCHAR ii,jj;
USHORT *p;
//
  if (EnableASD)
  {
    TakeSemaphore(s_asd);
    if (get_tickdur(ASDSendTick) > TimeToTick(ASDSendTime) )
    {
     tempbuf[0] = idASD;
     tempbuf[1] = idBUM;
     jj = NumOfTacts << 1;
     tempbuf[2] =  jj;
     tempbuf[3] =  0;
//
#ifdef OLD_PACKET_HEADER
     p = (USHORT*)&tempbuf[4];
#else
     tempbuf[5] =  0;
     p = (USHORT*)&tempbuf[6];
#endif
//
     for (ii=0; ii<NumOfTacts; ++ii)
     {
         *p++ = ASD_Buffer[ii];
         ASD_Buffer[ii] = ASDtype[ii];
     }
     putmsg(tempbuf,jj+hdrsize);
     ASDSendTick = xTaskGetTickCount();
    }
   xSemaphoreGive(s_asd);
  }
}
//-------------------------------------------------------------------
void UMU::setImitDPStep(unsigned char _imitDpStepValue, unsigned char DPNum )
{
    imitDPStep[DPNum] = _imitDpStepValue;
}
//-------------------------------------------------------------------
void UMU::fillDPMessageHdr(void *pDPMessage, BOOL extendedFormat)
{
register tLANMESSAGEHEADER *pHdr =  &((tDPCOORDMESSAGE*)pDPMessage)->Header;

/*
  pHdr->Sourse =  idBUM;
  pHdr->Id = idDPval;
  pHdr->Size =  szDPVal ;
#ifndef OLD_PACKET_HEADER
  pHdr->NotUse =  0;
#endif
*/
    if (extendedFormat  == FALSE)
    {
        fillMessageHeader(pHdr, idDPval, idBUM, szDPVal);
    }
        else
        {
            fillMessageHeader(pHdr, idDPvalEx, idBUM, szDPValEx);
        }
}
//-------------------------------------------------------------------
// DPNum -  абсолютный номер датчика смещения (ДП или имитатора)
void UMU::fillMessageByDPStepData(unsigned char DPNum, void *pDPMessage, unsigned char DPData, BOOL extendedFormat)
{
register  unsigned short *pBothFields; // Ї®«Ґ DPNumber ўла®ў­Ґ­® Ї® USHORT
register  unsigned char dpNumField;
   if (DPNum & IMITATORMASK)
   {
       dpNumField  = DPNum & ~IMITATORMASK | IMITSIGNMASK;
   }
       else
       {
           dpNumField  = DPNum;
       }
   if (extendedFormat == FALSE)
   {
       register tDPCOORDMESSAGE *msgPtr = (tDPCOORDMESSAGE*)pDPMessage;
         pBothFields = (USHORT *)&msgPtr->Data.DPNumber;
   }
       else
       {
          register tDPCOORDMESSAGEEX *msgPtr = (tDPCOORDMESSAGEEX*)pDPMessage;
          pBothFields = (USHORT *)&msgPtr->Data.DPNumber;
       }

  *pBothFields = (DPData << 8) |  dpNumField;

  if (DPData != 0)
  {
  volatile register unsigned int wasNegative;
    wasNegative = (unsigned int) (Pathcoord[DPNum] < 0);
    if (DPData & 0x80)
    { // negative offset
    volatile register unsigned char compliment;
        compliment = (DPData ^ 0xFF) + 1;
        Pathcoord[DPNum] -= compliment;
        if ( (!wasNegative) || (wasNegative) && (Pathcoord[DPNum] < 0) )
        {

            if (DPNum == mainShiftSensorNumber)
            {
                displayCoord += compliment;
            }
        }
            else
            {// имело место переполнение
                Pathcoord[DPNum] = MaxNegativePathCoord;
            }
    }
       else
       {
           Pathcoord[DPNum] += DPData;
           if ( (wasNegative) || (!wasNegative) && (Pathcoord[DPNum] > 0) )
           {
               if (DPNum == mainShiftSensorNumber)
               {
                   displayCoord += DPData;
               }
           }
               else
               {// имело место переполнение
                   Pathcoord[DPNum] = MaxPositivePathCoord;
               }
       }
  }
}

void UMU::moveLargeBScanInit()
{
    _BScanMessageHeader[0] =  (idBUM<<8)| idLargeBrazv;
    _BScanMessageHeader[1] = _BScanMessageHeader[2] = _BScanMessageHeader[3] = 0;
}

void UMU::moveLargeBScanBody(unsigned short *pHeader)
{
volatile register DWORD line = 0;
volatile register USHORT i, j, k, numSignalOffs;
unsigned short tactMask;
UCHAR fDPMsg;        // не равно 0, если было отослано сообщение ДП(имитатора)
//
    disintpld
// прерывания от PLD не должны открыться, если произойдет переключение на другую задачу из-за
// блокировки putmsg() и т.п
    COND_INT_ASD_ENABLE_LOCK
    START_BSCAN_PERIOD
#ifndef AC_dis
      if (ACCalcStage == ACTHCALC_CNTQUERY)
      {
          Wr_RegPLD(ACControlReg, ACControlValue_OFF); // чтобы привести автомат в исходное состояние
          ACCalcStage = ACTHCALC_WAITFORDATA;
          Wr_RegPLD(ACControlReg, ACControlValue_TUNE); // чтобы запустить новый цикл с выключенным ЗИ
          Wr_RegPLD(ACSumStartReg, ACSumStartValue);
      }
          else
          {
              if ( (ACCalcStage == ACTHCALC_WAITFORDATA) && (isACDataReady()) )
              {
// поиск максимальных значений сумм для всех значений сторона-линя-такт
              register unsigned short *pLinePLDData;
              register unsigned char tackt;
              register unsigned char line;
              register unsigned char side;
                  for (line = 0; line <= 1; ++line )
                  {
               if (line == 0)
               {
                   pLinePLDData = (USHORT*)(ACLineBlockStart_1);
               }
                   else
                   {
                       pLinePLDData = (USHORT*)(ACLineBlockStart_2);
                   }
               for (tackt=0; tackt < NumOfTacts; ++tackt)
               {
                   for (side=0; side <= 1; ++side)
                   {
                         (*pACMaxSums)[side][line][receiversInCycle[side][line][tackt]].Sums[0] = getAScanSumValueFromPLD(pLinePLDData,side);
//                            simplePrintf("\n takt = %d, side = %d, line = %d, receiver = %d, curval = 0x%x", tackt, side, line, receiversInCycle[side][line][tackt], (*pACMaxSums)[side][line][receiversInCycle[side][line][tackt]].Sums[0]);
                   }
                   pLinePLDData += szPLD_ACLineBlock;
              }
                  }
                  Wr_RegPLD(ACControlReg, ACControlValue_OFF); // оставим как есть, хотя новый цикл НЕ ЗАПУСТИТСЯ
                  ACCalcStage = ACTHCALC_COMPUTEQUERY;
              }
          }
#endif
//
       line = 0;
       fDPMsg = 0;

     if (shiftValue)
     {
//             simplePrintf("\nmoveBScan: num = %d, value = %d",mainShiftSensorNumber,shiftValue);
          sendDPData(mainShiftSensorNumber,shiftValue);
          fDPMsg = 1;
//
#ifndef AC_dis
         ACStateSendIntervalCntr += shiftValue;
         if (ACStateSendIntervalCntr & 0x80)
         {
             if (ACStateSendIntervalCntr <= cACStateSendInterval_Neg)
             {
                     fACStateSendIntervalReached = TRUE;
                     ACStateSendIntervalCntr =  (ACStateSendIntervalCntr ^ 0xFF) + 1;
                     ACStateSendIntervalCntr %= cACStateSendInterval_Pos;
                     ACStateSendIntervalCntr =  (ACStateSendIntervalCntr ^ 0xFF) + 1;
             }
          }
              else
              {
                  if (ACStateSendIntervalCntr >= cACStateSendInterval_Pos)
                  {
                         fACStateSendIntervalReached = TRUE;
                         ACStateSendIntervalCntr %= cACStateSendInterval_Pos;
                  }
              }
#endif
     }
         else if (fMScan == TRUE)
                 {
                     sendDPData(mainShiftSensorNumber,0);
                     fDPMsg = 1;
#ifndef AC_dis
                     fACStateSendIntervalReached = TRUE;
#endif
                 }
//
#ifndef AC_dis
      if ( (ACCalcStage == ACCALC_WAITFORDATA) && (gatheringSumCnt == 0) && isACDataReady() )
      {
        if ((isACDataChanged() || fACStateSendIntervalReached))
        {
          if (fACStateSendIntervalReached)
          {
              fACStateSendIntervalReached = FALSE;
          }
              else
              {
                  ACStateSendIntervalCntr = 0;
              }
          switch(enableACMessage)
          {
            case ACRAWDATA:
            {
                 register USHORT *pLine1Data;
                 register USHORT *pLine2Data;
                 register USHORT size;
                 USHORT thBlock[3];
                 unsigned char ii;

                     pLine1Data = (USHORT*)(ACLineBlockStart_1);
                     pLine2Data = (USHORT*)(ACLineBlockStart_2);
                     size = (aScanSumBlock_sz + aScanThBlock_sz ) *  NumOfTacts;
                     get_Access(size + sizeof(tLANMESSAGEHEADER));
                     ATTACH_MESSAGE_NUMBER_TO_HEADER(&aScanSumMsgHdr);
                     attachMessageLengthToHeader(&aScanSumMsgHdr,size);
                     put_DataByWord((USHORT*)&aScanSumMsgHdr,sizeof(tLANMESSAGEHEADER));
                     for (k=0;  k<NumOfTacts; ++k)
                     {
                         put_DataByWord(pLine1Data,aScanSumLineBlock_sz);
                         put_DataByWord(pLine2Data,aScanSumLineBlock_sz);
                         pLine1Data += szPLD_ACLineBlock;
                         pLine2Data += szPLD_ACLineBlock;
// в посылке требуются пороги для тактов, а не для приемников
                         for (ii=0; ii<3; ++ii)
                         {
                             thBlock[ii] = (*pAcThLine0)[receiversInCycle[indexLeft][0][k]][ii] & 0xFF00 | (*pAcThLine0)[receiversInCycle[indexRight][0][k]][ii] & 0xFF;
                         }
                         put_DataByWord((unsigned short*)thBlock, aScanThLineBlock_sz);
                         for (ii=0; ii<3; ++ii)
                         {
                             thBlock[ii] = (*pAcThLine1)[receiversInCycle[indexLeft][1][k]][ii] & 0xFF00 | (*pAcThLine1)[receiversInCycle[indexRight][1][k]][ii] & 0xFF;
                         }
                         put_DataByWord((unsigned short*)thBlock, aScanThLineBlock_sz);
                     }
                     release_Buffer;
            }
           case ACSTATE:
           {
               register USHORT *pLine1Data;
               register USHORT *pLine2Data;
               register USHORT aw;

                     pLine1Data = (USHORT*)(ACStateStartLine_1);
                     pLine2Data = (USHORT*)(ACStateStartLine_2);
                     aw = szAcContactStateBlock *  NumOfTacts;
                     get_Access(aw + sizeof(tLANMESSAGEHEADER));
                     ATTACH_MESSAGE_NUMBER_TO_HEADER(&acStateMsgHdr);
                     attachMessageLengthToHeader(&acStateMsgHdr,aw);
                     put_DataByWord((USHORT*)&acStateMsgHdr,sizeof(tLANMESSAGEHEADER));
                     for (k=0;  k<NumOfTacts; ++k)
                     {
                         aw = *pLine1Data;
                         aw |= *pLine2Data << 4;
                         put_DataByWord(&aw,2);
                         pLine1Data += szPLD_ACLineBlock;
                         pLine2Data += szPLD_ACLineBlock;
                     }
                     release_Buffer;
               break;
            }
         }
        }
        Wr_RegPLD(ACSumStartReg, ACSumStartValue);
        ACCalcStage = ACCALC_NEED_TO_START;
      }
#endif
//
  if (fDPMsg)
  {
// когда идет сбор сумм, эта задача возобновляется в т.ч тогда, когда нет повода для B(М)-развертки
// поэтому исполняем код только, если fDPMsg
      while(line < 2)
      {
         volatile register unsigned short NumOfSignals;
         if (line == 0) Wr_RegPLD(BScanASDCRegAddr, 0x202); // «Ё­Ёп 0
                 else  Wr_RegPLD(BScanASDCRegAddr, 0x303);
//
         for (k=0, tactMask = 0; k<NumOfTacts; ++k)
         {
//            NumOfSignals = *(USHORT*)(BScanASD_0+(k<<7));
              NumOfSignals = Rd_RegPLD(BScanASD_0 + (k<<7));

//               simplePrintf("\n line = %d, takt = %d,signals = 0x%x",line,k,NumOfSignals);
//
            if (NumOfSignals != 0)
            {
                if ((NumOfSignals & 0xFF00) > 0x800)
                {
                   NumOfSignals &= 0xFF;
                   NumOfSignals |= 0x800;
                }
                if ((NumOfSignals & 0xFF) > 0x8)
                {
                  NumOfSignals &= 0xFF00;
                  NumOfSignals |= 0x8;
                }
                j = MAX(NumOfSignals >> 8, NumOfSignals & 0xFF);

                register USHORT a  = j * bScanSignalBlock_sz +2;

                get_Access(a+hdrsize);
#ifdef OLD_PACKET_HEADER
              pHeader[1] = a;
              pHeader[2] =  ((NumOfSignals & 0xF00) << 4) | ((NumOfSignals & 0xF) << 8)  | (line<<6) | k;
#else
              pHeader[1] = a;
              ATTACH_MESSAGE_NUMBER(*(UCHAR*)&pHeader[2])
              pHeader[3] =  ((NumOfSignals & 0xF00) << 4) | ((NumOfSignals & 0xF) << 8)  | (line<<6) | k;
#endif

              put_DataByWord(pHeader, sizeof(_BScanMessageHeader));
              numSignalOffs = 0;
              for (i=0; i<j; ++i)
              {
#ifndef DEVICE_EMULATION
                 pw = (USHORT*)(BScanASD_1 + tactMask + numSignalOffs);
                 put_DataByWord(pw,bScanSignalBlock_sz);
#else
                 put_DataByWord(BScanASD_1 + tactMask + numSignalOffs, bScanSignalBlock_sz);
#endif
                 numSignalOffs += (bScanSignalBlock_sz+2);
              }
              release_Buffer;
          }
          tactMask += cTactMaskStep;
         } // for
//
          line++;
      } // while
  }
  if ((fDPMsg) || (fMScan)) initMScanFlags();


/*
#ifndef DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
         if (INT_T1_PENDING)
#else
         if ( PLD_INT_PIN_STATE == 0)
#endif
         {

         }
*/
        COND_INT_ASD_ENABLE_UNLOCK

        SUSPEND_ISR_TASK();
}

void UMU::moveLargeBScan(void *ppar)
{
    if ((fMScan) || (shiftValue))
    {
        moveLargeBScanBody(_BScanMessageHeader);
    }
}
//-----------------------------------------------------------------------------------------------------------
void UMU::initMScanFlags(void)
{
      fResetMScanTimer = 1; // ¤«п ЇҐаҐ§ ЇгбЄ  в ©¬Ґа
       fMScan = 0;
}
//-----------------------------------------------------------------------------------------------------------
void UMU::mScanAlarm(void)
{
    if(fResetMScanTimer)
    {
        MScanSendTick = xTaskGetTickCount();
        fResetMScanTimer = 0;
    }
        else
            if  ((!fMScan) && (get_tickdur(MScanSendTick) > TimeToTick(MScanSendTime) ))
            {
                     fMScan = 1;
            }
}
//-----------------------------------------------------------------------------------------------------------
void UMU::setRelayCtrlSignals(unsigned char command)
{
    switch(command)
    {
        case setKCmd:
        case resetKCmd:
           Wr_RegPLD(a0x1307, command);
           vTaskDelay(TimeToTick(cRelayCtrlSignalDur));
           Wr_RegPLD(a0x1307, KSignalsOff);
            break;
        default:
           Wr_RegPLD(a0x1307, KSignalsOff);
    }
}
//-----------------------------------------------------------------------------------------------------------
void UMU::setDPOption(unsigned short value)
{
    Wr_RegPLD(DPCONTROLREGADR , value);
}
//-----------------------------------------------------------------------------------------------------------
void UMU::setTrolleyDP()
{
    if (lengthPathEncoderDividerOffPar == 1) setDPOption(0);
        else setDPOption(SETDIVIDER3BIT);
    fIsTrolleyDP = TRUE;
}
//-----------------------------------------------------------------------------------------------------------
void UMU::setScanerDP()
{
    setDPOption(SETSCANERDPMAJORBIT);
    fIsTrolleyDP = FALSE;
}
//-----------------------------------------------------------------------------------------------------------
void UMU::KSwitch(UCHAR fScanerOn)
{
  if (fScanerOn)
  {
      setScanerDP();
      setRelayCtrlSignals(setKCmd);
  }
     else
     {
         setTrolleyDP();
         setRelayCtrlSignals(resetKCmd);
     }
  REDEFINE_DP_CYCLE_PROC
}
//-----------------------------------------------------------------------------------------------------------
void UMU::scanerSwitch(UCHAR *p)
{
  smprintf("\nscanerSwitch: ");
  if (*p & 1)
  { // Ї®¤Є«озЁвм бЄ ­Ґа
      smprintf("switch to scaner Av17");
      KSwitch(setScanerOn);
  }
     else
     { // Є ­ «л бЇ«®и­®Ј® Є®­ва®«п
         smprintf("switch to trolley's channels");
         KSwitch(setScanerOff);
     }
}

// dataLength - длина данных в pmsg, значение не меньше hdrsize
// возвращает:
// положительное число - длина обработанной команды вместе с заголовком
// иначе:
// -1 - неизвестный код команды
// -2 - команда принята не целиком

tres UMU::parcer(UCHAR* pmsg,USHORT dataLength)
{
tres res;

     switch ( ReadLE32U(pmsg) & 0xFFFF00FF)
     {
        case Gc(idNtact,szNtact):
        {
             res =  szNtact+hdrsize;
             if (dataLength < res) res = -2;
                 else SetNumOfTacts(pmsg+hdrsize);
             break;
        }
//
        case Gc(idTactpar,szTactpar):
        {
             res =  szTactpar+hdrsize;
             if (dataLength < res) res = -2;
                 else SetTacktParam((tTACTPARAMSCMD*)(pmsg+hdrsize));
             break;
        }
//
        case Gc(idVRU,szVRU):
        {
             res =  szVRU+hdrsize;
             if (dataLength < res) res = -2;
                 else ChangeVRU(pmsg+hdrsize);
             break;
         }
//
        case Gc(idStrobpar,szStrobpar):
        {
             res =  szStrobpar+hdrsize;
             if (dataLength < res) res = -2;
                 else ChangeStrobs(pmsg+hdrsize);
             break;
        }
//
        case Gc(id2Tp,sz2Tp):
        {
             res =  sz2Tp+hdrsize;
             if (dataLength < res) res = -2;
                 else Change2Tp(pmsg+hdrsize);
             break;
         }
//
        case Gc(idOnAscan,szOnAscan):
        {
             res =  szOnAscan+hdrsize;
             if (dataLength < res) res = -2;
                 else StartAScan(pmsg+hdrsize);
             break;
        }
//
        case Gc(idBUMctrl,szBUMctrl):
        {
             res =  szBUMctrl+hdrsize;
             if (dataLength < res) res = -2;
                 else BUMctrlproc(pmsg+hdrsize);
             break;
        }
//
        case Gc(idDPset,szDPset):
        {
             res =  szDPset+hdrsize;
             if (dataLength < res) res = -2;
                 else SetDPval(pmsg+hdrsize);
             break;
        }
//
        case Gc(idDPim,szDPim):
        {
             res =  szDPim+hdrsize;
             if (dataLength < res) res = -2;
                 else setDPImitCmd(pmsg+hdrsize);
             break;
        }
//
        case Gc(idKSwitch,szKSwitch):
        {
             res =  szKSwitch+hdrsize;
             if (dataLength < res) res = -2;
                 else scanerSwitch(pmsg+hdrsize);
             break;
        }
//
        case Gc(idDPimEx,szDPimEx):
        {
             res =  szDPimEx+hdrsize;
             if (dataLength < res) res = -2;
                 else setDPImitExCmd(pmsg+hdrsize);
             break;
        }
//
        case Gc(idDPScanImit,szDPScanImit):
        {
             res =  szDPScanImit+hdrsize;
             if (dataLength < res) res = -2;
                 else setScanImitCmd(pmsg+hdrsize);
             break;
        }
//
        case Gc(idBScanSignalsCutStart,szBScanSignalsCutStart):
        {
             res =  szBScanSignalsCutStart+hdrsize;
             if (dataLength < res) res = -2;
                 else setBScanSignalsCutStartCmd((tBSCANSIGNALSCUTSTARTCMD*)(pmsg+hdrsize));
             break;
        }
//
        case Gc(idDefAScanSumTh,szDefAScanSumTh):
        {
             res =  szDefAScanSumTh+hdrsize;
             if (dataLength < res) res = -2;
#ifndef AC_dis
                 else defAScanSumThCmd(pmsg+hdrsize);
#endif
             break;
        }
//
/*
        case Gc(idSetAScanSumTh,szSetAScanSumTh):
        {
             res =  szSetAScanSumTh+hdrsize;
             if (dataLength < res) res = -2;
                 else setAScanSumThCmd(pmsg+hdrsize);
             break;
        }
*/
//
        default:
        res = -1;
        simplePrintf("\n us46_parcer: unknown COP: ");
//
//ifdef _us46_cpp_prn
        {
          UCHAR ii,*p;

          for (ii = 0, p = pmsg; ii< 8;ii++,p++) smprintf(" %x", *p);
        }
//endif
     }
     return res;
}
//--------------------------------------------------------------------
//
unsigned char UMU::lanmsgparcer(UCHAR* buf, USHORT lng)
{
register int res = 0;
register int l;
register UCHAR *p = buf;
register unsigned char receiveStartOffset;
//
  l = lng;
  while (l > hdrsize)
  {
   res = parcer(p,l);
   if (res > 0)
   {
     p += res;
     l -= res;
   }
     else
     {
       if (res == -1)
       {
           p++;
           l--;
       }
           else break;
     }
  }
  if ((l != 0) || (res == -2))
  {
    int ii;
    if (res == -2) smprintf("\nlanmsgparcer: parser returned an error -2");

    smprintf("\nlanmsgparcer: income data has the unparced %d bytes:\n",l);

    if (l > cReceiveStartOffsetMax) simplePrintf("\nlanmsgparcer: too many unparsed bytes - %d",l); // очень вероятно, что при приеме следущей порции
// данных часть их будет утрачена - см. buvrcvtsk()
//
    receiveStartOffset  = (UCHAR)l;
    for (ii = 0; ii<l; ii++)
    {
      buf[ii] = p[ii];
      smprintf(" 0x%x", p[ii]);
    }
  }
     else receiveStartOffset = 0;
  return receiveStartOffset;
}
//----------------------------------------------------------------------

void UMU::varsinit(void)
{
register UCHAR ii;
//
   NumOfTacts=1;
   fIntPLDMustBeEnable = 0;
   AscanSendTick[0] =  AscanSendTick[1] = \
   ASDSendTick = MScanSendTick = xTaskGetTickCount();

   for (ii=0; ii<4; ii++) ascan[ii] = 0;
//
   EnableASD = 0;
//
   for (ii=0; ii<tactbitmsk+1; ii++)
   {
     ASDtype[ii] = 0;
   }
   EnableBScan = 0;
//
#ifndef AC_dis
   enableACMessage = ACDISABLED;
#endif
//
#ifdef LARGE_BSCAN
  shiftValue = 0;
#ifndef AC_dis
  ACStateSendIntervalCntr = 0;
  fACStateSendIntervalReached = FALSE;
#endif
  initMScanFlags();
#endif

#ifndef OLD_PACKET_HEADER
    messageNumber = 0;
#endif

   DP_SEMAPHORE_ENTER
   setDPOption(STOP_DP_MACHINE_BIT);
   vTaskDelay(1);
   mainShiftSensorNumber = 0;
   for (ii=0; ii<QDP;ii++) pShiftSensorProc[ii] = NULL;

   for (ii=0; ii<QDP-1;ii++)
   {
       setImitDPStep(DPSTEPFORWARD,ii);
       imitDPIncTime[ii] = minImitDPIncTime;
       imitDPIncTick[ii] = xTaskGetTickCount();
      doWhenImitSwitchOFF(ii);

   }
   for (ii=0; ii<QDP*2; ii++) Pathcoord[ii] = 0;
   lDiff = 0;
   DP_SEMAPHORE_LEAVE
//
   KSwitch(setScanerOff);

#ifndef AC_dis
   pACMaxSums = NULL;
#endif

//   ACThCalcInit();
}
//-------------------------------------------------------------------
// ptr points to message body
//
void UMU::BUMctrlproc(UCHAR *ptr)
{
// smprintf("\nBUMctrlproc enter");
   switch(ReadLE16U(ptr))
   {
     case 0x43:
          PLDEn();
          break;
     case 0x44:
          PDLDis();
          break;
     case 0x46:
          StopAScan();
          break;
     case 0x47:
          StartBScan();
          break;
     case 0x48:
          StopBScan();
          break;
     case 0x4B:
          ASD_On();
          break;
     case 0x4C:
          ASD_Off();
          break;
     case 0xC2:
          Get_Uakk();
          break;
     case 0xC3:
          make_C3();
          break;
//
     case 0xDE:
          SendVersionInfo() ;
          versionInfoSendTick =  xTaskGetTickCount();
          break;
//
     case 0xF0:
          ReloadBUM();
          break;
//
     case 0x35:
         setExendedFormatOfDPValMessageCmd(TRUE);
         break;
//
     case 0x36:
         setExendedFormatOfDPValMessageCmd(FALSE);
         break;
#ifndef AC_dis
     case 0x37:
          ascanSumOn();
           break;
     case 0x38:
          acMessageOff();
          break;
     case 0x39:
          acStateOn();
           break;
     case 0x88:
         getAScanSumThCmd();
         break;
#endif
   }
}
//------------------------------------------------------------
void UMU::ASD_On(void)
{
  if (EnableBScan)
  {
      TakeSemaphore(s_asd); // precaution: if we are reenabling ASD
      EnableASD = 1;
      ASDSendTick = xTaskGetTickCount();
      xSemaphoreGive(s_asd);
  }
}
//-------------------------------------------------------------------
void UMU::ASD_Off(void)
{
  EnableASD = 0;
}
//-------------------------------------------------------------------
void UMU::make_C3(void)
{
  O1Data[0] = idTime;
  O1Data[1] = idBUM;
  O1Data[2]  = szTime;
  O1Data[3]  = 0;
//
  WriteLE32U(&O1Data[hdrsize],xTaskGetTickCount() >> 1); // для кванта 500 мкС
  O1Data[5]  = 0;
  putmsg(O1Data,szTime+hdrsize);
}

//-------------------------------------------------------------------
void UMU::SendVersionInfo(void)
{
register USHORT devicenumber;
//  fill_sftversion
    O1Data[0]= idSftw;
    O1Data[1]= idBUM;
    O1Data[2] = szSftw;
    O1Data[3] = 0;
//
    O1Data[hdrsize] = (version_number) & 0xFF;
    O1Data[hdrsize+1] = (version_number >> 8) & 0xFF;
    O1Data[hdrsize+2] = (version_number >> 16) & 0xFF;
    O1Data[hdrsize+3] = PLDVerId  & 0xFF;
    O1Data[hdrsize+4] = (PLDVerId >> 8) & 0xFF;
    O1Data[hdrsize+5] = (PLDVerId >> 16) & 0xFF;
    O1Data[hdrsize+6] = PLDVerId >> 24;
    O1Data[hdrsize+7] = 0;
    O1Data[5]  = 0;
    putmsg(O1Data,szSftw+hdrsize);
//
//  fill_serialnumber
    O1Data[0]= idDevnum;
    O1Data[1]= idBUM;
    O1Data[2]= szDevnum;
    O1Data[3]= 0;
    devicenumber = get_devicenumber();
    if (devicenumber != 0) WriteLE16U(&O1Data[hdrsize],devicenumber);
      else  WriteLE16U(&O1Data[hdrsize],0xFFFF); // not to be zero
    putmsg(O1Data,szDevnum+hdrsize);
}

//-------------------------------------------------------------------
// !
// ptr points to message body
// see lim1
void UMU::StartAScan(UCHAR *ptr)
{
register UCHAR idx;

  TakeSemaphore(s_ascan);
//
#ifdef ONE_CHANNEL_ASCAN_ONLY
  for (idx = 0; idx < 4; ++idx)
  {// отключить ранее включенное
      ascan[idx] = 0;
  }
#endif
//
  idx =  (ptr[slt_bidx] & (linebitmsk | sidebitmsk)) >> 6;
  ascan[idx]=1;
  curchan[idx]=ptr[slt_bidx] & tactbitmsk;
  ascanstart[idx]=ptr[slt_bidx+1];
  ascanscale[idx]=ptr[slt_bidx+2];
  ascanregim[idx]=ptr[slt_bidx+3];
  ascanstrobformax[idx]=ptr[slt_bidx+4] & 3;
  if (get_tickdur(AscanSendTick[idx & 1]) > TimeToTick(AscanSendTime))
       AscanSendTick[idx & 1] = xTaskGetTickCount();


  ascancounter = 0;

  xSemaphoreGive(s_ascan);
//
  smprintf("\nStartAScan finished - idx=%d ascan[] =%x",idx,ascan[idx]);

}
//-------------------------------------------------------------------
// установка значения датчика пути или имитатора
void UMU::SetDPval(UCHAR *p)
{
register unsigned char sensorNum;
register unsigned char mem;
//
  smprintf("\nSetDPval enter: ");

    sensorNum = *p & SHIFTSENSORNUMBERMASK;
    mem = *p;
    if (*p & IMITSIGNMASK)
    {
             sensorNum |=  IMITATORMASK;
    }

    if (*p++ & MAINSENSORSIGNMASK)
    {
        DP_SEMAPHORE_ENTER
        disintpld;
        mainShiftSensorNumber = sensorNum;
        displayCoord = 0;
        COND_IntASD_ENABLE
        redefineDPCycleProc();
        DP_SEMAPHORE_LEAVE
        smprintf("\nSetDPval: DP number = %d became main one ",sensorNum);
    }
        else
        {
            DP_SEMAPHORE_ENTER
            disintpld;
            Pathcoord[sensorNum] = ReadLE32U(p);
            if (mainShiftSensorNumber == sensorNum)
            {
                displayCoord = 0;
                mainShiftSensorPathcoordMemorized = Pathcoord[mainShiftSensorNumber];
                speedTimer = xTaskGetTickCount();
            }
            COND_IntASD_ENABLE
            WriteLE32U(&O1Data[hdrsize+1], sensorNum);
            DP_SEMAPHORE_LEAVE
            smprintf("\nSetDPval: DP number = %d. Path coordinate %d was set",sensorNum,Pathcoord[sensorNum]);
//
            O1Data[0] = idDPset;
            O1Data[1] = idBUM;
            O1Data[2]  = szDPValAck;
            O1Data[3]  = 0;
//
            O1Data[hdrsize] = mem;
            O1Data[5]  = 0;
            putmsg(O1Data,szDPValAck+hdrsize);
         }
}
//-------------------------------------------------------------------
void UMU::setDPImitCmd(UCHAR *p)
{
  smprintf("\nsetDPImitCmd enter: ");
  setImit(p,FALSE);
}
//-------------------------------------------------------------------
// д.б. пройден семафор     DP_SEMAPHORE_xxx
void UMU::doWhenImitSwitchOFF(unsigned char relativeNum)
{
register unsigned char ii;

    ImitDPOn[relativeNum] = IMITDPOFF;
    redefineDPCycleProc();
// чтение ДП ПЛИС, чтобы удалить данные, т.к. колесо уже могли покрутить
// читаем 3 раза, чтобы очистить счетчик, хотя достаточно и двух
    for (ii = 0; ii < 3; ii++)
    {
        getDPShift(relativeNum);
        vTaskDelay(1);
    }
}
//-------------------------------------------------------------------
// д.б. пройден семафор DP_SEMAPHORE_xxx
void UMU::doWhenScanerImitSwitchOFF(void)
{
    doWhenImitSwitchOFF(SCANERHEIGHTSENSORNUMBER);
    doWhenImitSwitchOFF(SCANERLENGTHSENSORNUMBER);
}
//-------------------------------------------------------------------
void UMU::setImit(unsigned char *p,unsigned char fExtendedCmd)
{
register unsigned char relativeNum;
//gister int res;
tDPIMITCMDEX *pData;

  smprintf("\nsetImit enter: ");
  pData = (tDPIMITCMDEX*)p;

  relativeNum = pData->Main.ImitNumber & SHIFTSENSORNUMBERMASK; // маска - на всякий случай
  if ((relativeNum == SCANERHEIGHTSENSORNUMBER) && (ImitDPOn[SCANERHEIGHTSENSORNUMBER] == IMITDPSPECMODE1))
  { // невозможно включить/отключить такой командой имитатор когда уже включен имитатор сканера
      smprintf("error: imitator DP(%d) command can not be used i.e. scaner imitator is already active",pData->Main.ImitNumber);
      return;
  }
  if ((relativeNum == SCANERLENGTHSENSORNUMBER)&&(ImitDPOn[SCANERHEIGHTSENSORNUMBER] == IMITDPSPECMODE1))
  { // невозможно включить/отключить такой командой имитатор когда уже включен имитатор сканера
      smprintf("error: imitator DP(%d) is main and can not be switched on or off  i.e. scaner imitator s already active",pData->Main.ImitNumber);
      return;
  }
//
    DP_SEMAPHORE_ENTER

#ifndef DEVICE_EMULATION
#ifndef us46emu
  T1TCR = 0;  // to avoid FIQ by this way
#endif
#endif

    switch(pData->Main.State)
    {
        case 0:
        {
            disintpld // т.к. в прерывании семафор не поставишь
            doWhenImitSwitchOFF(relativeNum);
            COND_IntASD_ENABLE
            break;
        }
        case 1:
        {
        unsigned short interval;
            interval = minImitDPIncTime;
            if (fExtendedCmd)
            {
                unsigned short intervalValue;
                intervalValue = ReadLE16U((UCHAR*)pData->EventInterval); // поле м.б. не выравнено по word
                if  (interval < intervalValue) interval = intervalValue;
            }
            imitDPIncTime[relativeNum] = interval;
            ImitDPOn[relativeNum] = IMITDPWAIT;

            redefineDPCycleProc();

#ifdef LARGE_BSCAN
           shiftValue = 0;
#endif
          break;
         }
    }
//
#ifndef DEVICE_EMULATION
#ifndef us46emu
  T1TCR = 1;  //
#endif
#endif

    DP_SEMAPHORE_LEAVE
    smprintf("ImitDP(%d) new state = %d",relativeNum, pData->Main.State);

}
//-------------------------------------------------------------------
void UMU::setDPImitExCmd(UCHAR *p)
{
  smprintf("\nsetDPImitExCmd enter: ");
  setImit(p,TRUE);
}
//-------------------------------------------------------------------
void UMU::setScanImitCmd(UCHAR *p)
{
tDPSCANIMITCMD *pData;

    pData = (tDPSCANIMITCMD *)p;
    smprintf("\nsetScanImitCmd enter: ");

    if (pData->AParameter == 0)
    {
        smprintf("error: parameter A equals 0");
        return;
    }
    if (pData->BParameter == 0)
    {
        smprintf("error: parameter B equals 0");
        return;
    }
//
    if ((ImitDPOn[SCANERHEIGHTSENSORNUMBER] != IMITDPOFF) &&  (ImitDPOn[SCANERHEIGHTSENSORNUMBER] != IMITDPSPECMODE1))
    {
        smprintf("error: scaner imitator command can not be used i.e. imitator DP(%d) is active",SCANERHEIGHTSENSORNUMBER);
        return;
    }
//
    if ((ImitDPOn[SCANERHEIGHTSENSORNUMBER] != IMITDPSPECMODE1) && (ImitDPOn[SCANERLENGTHSENSORNUMBER] != IMITDPOFF))
    {
        smprintf("error: scaner imitator command can not be used i.e. imitator DP(%d) is active",SCANERLENGTHSENSORNUMBER);
        return;
     }
    DP_SEMAPHORE_ENTER

    mainShiftSensorNumber |=  IMITATORMASK | SCANERLENGTHSENSORNUMBER;

#ifndef DEVICE_EMULATION
#ifndef us46emu
  T1TCR = 0;  // to avoid FIQ by this way
#endif
#endif


    switch(pData->State)
    {
        case 0:
            doWhenScanerImitSwitchOFF();
            break;
//
        case 1:
        {
        unsigned short interval;
            interval = ReadLE16U((unsigned char*)&pData->EventInterval); // поле м.б. не выравнено по word
            if  (minImitDPIncTime > interval)  interval = minImitDPIncTime;

            mainShiftSensorNumber |=  IMITATORMASK | SCANERLENGTHSENSORNUMBER;
            imitDPIncTime[mainShiftSensorNumber & SHIFTSENSORNUMBERMASK] = interval;
            ImitDPOn[mainShiftSensorNumber & SHIFTSENSORNUMBERMASK] = IMITDPWAIT;
            ImitDPOn[SCANERHEIGHTSENSORNUMBER] = IMITDPSPECMODE1;

            parameterA = pData->AParameter;
            parameterB = pData->BParameter;
            scanerImitStage =  SCANIMITINIT;

//            pShiftSensorProc[SCANERHEIGHTSENSORNUMBER] = NULL;   // на всяк случай
//            pShiftSensorProc[SCANERLENGTHSENSORNUMBER] = scanerImitProc;
            break;
        }
    }

#ifndef DEVICE_EMULATION
#ifndef us46emu
  T1TCR = 1;  //
#endif
#endif

    DP_SEMAPHORE_LEAVE
}

//-------------------------------------------------------------------
// side - SideLeft/SideRight
//
void UMU::make_jointSensor(UCHAR side,BOOL newState)
{
  O1Data[0] = idJAndASensor;
  O1Data[1] = idBUM;
  O1Data[2]  = szJAndASensor;
  O1Data[3]  = 0;
//

#ifdef OLD_PACKET_HEADER
  O1Data[4]  = side;
  O1Data[5]  = jointSensorId;
  if (newState)  O1Data[6]  = cJointSensorJump;
      else O1Data[6]  = cJointSensorFall;
  putmsg(O1Data,szJAndASensor+hdrsize,NULL);
#else
  O1Data[5]  = 0;
  O1Data[6]  = side;
  O1Data[7]  = jointSensorId;
  if (newState)  O1Data[8]  = cJointSensorJump;
      else O1Data[8]  = cJointSensorFall;

  putmsg(O1Data,szJAndASensor+hdrsize);
#endif

}

//-------------------------------------------------------------------
// д.б. пройден семафор DP_SEMAPHORE_xxx
// imitNumber - относительный
void UMU::imitWriteDownCycleProc(UCHAR imitNumber)
{
    disintpld
    if  (get_tickdur(imitDPIncTick[imitNumber]) > (TimeToTick(imitDPIncTime[imitNumber])) )
    {
        imitDPIncTick[imitNumber] = xTaskGetTickCount();
        if (ImitDPOn[imitNumber] > IMITDPOFF)
        {
            if (ImitDPOn[imitNumber] <  IMITDPSENDL)
            {
                ImitDPOn[imitNumber]++;
            }
        }
    }
    COND_IntASD_ENABLE
}
//-------------------------------------------------------------------
void UMU::pathEncoderCycleProc(unsigned char relativeNumber)
{
unsigned char value;
    value = getDPShift(relativeNumber);
    if (value != 0)
    {
//        simplePrintf("\npathEncoderCycleProc: num = %d, value = %d",relativeNumber,value);

        sendDPData(relativeNumber,value);
    }
}
//-------------------------------------------------------------------
void UMU::trolleyPathCycleProc(void)
{
    pathEncoderCycleProc(TROLLEYSENSORNUMBER);
}
//-------------------------------------------------------------------
void UMU::scanerLCycleProc(void)
{
    pathEncoderCycleProc(SCANERLENGTHSENSORNUMBER);
}
//-------------------------------------------------------------------
void UMU::scanerHCycleProc(void)
{
    pathEncoderCycleProc(SCANERHEIGHTSENSORNUMBER);
}
//-------------------------------------------------------------------
void UMU::imitCycleProc(unsigned char relativeNumber)
{
register unsigned char value;

    value = getImitShift(relativeNumber);
    if (value != 0)
    {
        sendDPData(relativeNumber | IMITATORMASK,imitDPStep[relativeNumber]);
    }
}
//-------------------------------------------------------------------
void UMU::imitTrolleyPathCycleProc(void)
{
    imitCycleProc(TROLLEYSENSORNUMBER);
}
//-------------------------------------------------------------------
void UMU::imitScanerLCycleProc(void)
{
    imitCycleProc(SCANERLENGTHSENSORNUMBER);
}
//-------------------------------------------------------------------
void UMU::imitScanerHCycleProc(void)
{
    imitCycleProc(SCANERHEIGHTSENSORNUMBER);
}
//-------------------------------------------------------------------
// д.б. пройден семафор DP_SEMAPHORE_xxx
// вызывать при переходе с датчика пути на ОБЫЧНЫЙ имитатор и наоборот
void UMU::redefineDPCycleProc(void)
{
unsigned char ii;
   for (ii=0; ii<QDP-1;ii++)
   {
       if (ImitDPOn[ii] == IMITDPOFF)
       {
           if (
                ((mainShiftSensorNumber & IMITATORMASK) == 0) &&  ( ii ==  (mainShiftSensorNumber & SHIFTSENSORNUMBERMASK))  \
                 ||  (fIsTrolleyDP == FALSE)  && (ii == TROLLEYSENSORNUMBER)  \
                 ||  (fIsTrolleyDP == TRUE)  && (ii == SCANERLENGTHSENSORNUMBER)  \
              )
           {
               pShiftSensorProc[ii] = NULL;
           }
               else
               {
//                   switch(ii)
//                   {
//                       case TROLLEYSENSORNUMBER:
//                           pShiftSensorProc[ii] = trolleyPathCycleProc;
//                           break;
//                       case SCANERLENGTHSENSORNUMBER:
//                           pShiftSensorProc[ii] = scanerLCycleProc;
//                           break;
//                       case SCANERHEIGHTSENSORNUMBER:
//                           pShiftSensorProc[ii] = scanerHCycleProc;
//                           break;
//                       default:
//                           pShiftSensorProc[ii] = NULL;
//                   }
               }
       }
           else
           { // этот имитатор включен
               if ( (mainShiftSensorNumber & IMITATORMASK) &&  ( ii ==  (mainShiftSensorNumber & SHIFTSENSORNUMBERMASK)) )
               {
                   pShiftSensorProc[ii] = NULL;
               }
                  else
                  {
//                       switch(ii)
//                       {
//                           case TROLLEYSENSORNUMBER:
//                               pShiftSensorProc[ii] = imitTrolleyPathCycleProc;
//                               break;
//                           case SCANERLENGTHSENSORNUMBER:
//                               pShiftSensorProc[ii] = imitScanerLCycleProc;
//                               break;
//                           case SCANERHEIGHTSENSORNUMBER:
//                               pShiftSensorProc[ii] = imitScanerHCycleProc;
//                               break;
//                           default:
//                               pShiftSensorProc[ii] = NULL;
//                       }
                 }
           }
   }
}
//-------------------------------------------------------------------
// при работе совместно с основным ДП
void UMU::scanerImitProc(void)
{
   if (scanerImitStage == SCANIMITINIT)
   {
            paramACounter = parameterA;
            scanerImitStage =  SCANIMITINC1;
            setImitDPStep(DPSTEPFORWARD,SCANERLENGTHSENSORNUMBER);
   }
       else
       {
           if (ImitDPOn[SCANERLENGTHSENSORNUMBER] == IMITDPWAIT)
           {
               switch (scanerImitStage)
               {
                   case SCANIMITINC1:
                   {
                       paramACounter--;
                       if (paramACounter == 0)
                       {
                           paramACounter = parameterA << 1;
                          setImitDPStep(DPSTEPBACKWARD,SCANERLENGTHSENSORNUMBER);
                          scanerImitStage =  SCANIMITDEC;
                       }
                       break;
                   }
                  case SCANIMITDEC:
                  {
                       paramACounter--;
                       if (paramACounter == 0)
                       {
                           paramACounter = parameterA;
                          setImitDPStep(DPSTEPFORWARD,SCANERLENGTHSENSORNUMBER);
                          scanerImitStage =  SCANIMITINC2;
                       }
                      break;
                  }
                  case SCANIMITINC2:
                  {
                       paramACounter--;
                       if (paramACounter == 0)
                       {
                           sendDPData(SCANERHEIGHTSENSORNUMBER | IMITATORMASK , imitDPStep[SCANERHEIGHTSENSORNUMBER]);
                           parameterB--;
                          if (parameterB)
                          {
                              paramACounter = parameterA;
                              scanerImitStage =  SCANIMITINC1;
                          }
                              else doWhenScanerImitSwitchOFF();       //  выход из имитатора сканера
                       }
                      break;
                  }
               }
            }
        }
   imitScanerLCycleProc();

}
//-------------------------------------------------------------------
void UMU::sendDPData(unsigned char sensorNum, unsigned char sensorData)
{
    if ((fDPCoordDataExtended != 0) && (sensorNum == mainShiftSensorNumber) )
    {
        fillDPMessageHdr((void*)&DPMessage, TRUE);
        fillMessageByDPStepData(sensorNum, (void*)&DPMessage, sensorData, TRUE);
        WriteLE32U((unsigned char*)&DPMessage.Data.PathCoord, (DWORD)Pathcoord[sensorNum]);
        WriteLE32U((unsigned char*)&DPMessage.Data.DisplayCoord, displayCoord);
//
        putmsg((unsigned char*)&DPMessage, szDPCoordMessageEx);
    }
        else
        {
            register tDPCOORDMESSAGE *messagePtr = (tDPCOORDMESSAGE*)&DPMessage;
            fillDPMessageHdr((void*)&DPMessage, FALSE);
            fillMessageByDPStepData(sensorNum, (void*)&DPMessage, sensorData, FALSE);
            WriteLE32U((unsigned char*)&messagePtr->Data.PathCoord,(DWORD)Pathcoord[sensorNum]);
//
            putmsg((unsigned char*)&DPMessage, szDPCoordMessage);
        }
}
//-------------------------------------------------------------------
unsigned char UMU::getDPShift(UCHAR relativeNumber)
{
volatile register unsigned char val;
// признак того, что в outreg[] есть данные для чтения устанавливается по
// восходящему фронту /rd. Так что придется почитать еще раз, если считали ноль
     val = GET_DPx_VALUE(relativeNumber);
     if (val == 0) val = GET_DPx_VALUE(relativeNumber);
     return val;
}
//-------------------------------------------------------------------
unsigned char UMU::getImitShift(unsigned char relativeNumber)
{
unsigned char res;
    if  (ImitDPOn[relativeNumber] > IMITDPWAIT)
    {
       res = imitDPStep[relativeNumber];
       ImitDPOn[relativeNumber] = IMITDPWAIT;
    }
        else res = 0;
    return res;
}
//-------------------------------------------------------------------
unsigned char UMU::getShiftSensorValue(unsigned char number)
{
    if (number & IMITATORMASK)
    {
        return getImitShift(number & SHIFTSENSORNUMBERMASK);
    }
        else
        {
            if ( (number == TROLLEYSENSORNUMBER ) && (fIsTrolleyDP == TRUE) || (number == SCANERLENGTHSENSORNUMBER ) && (fIsTrolleyDP == FALSE)  || (number == SCANERHEIGHTSENSORNUMBER ))
            {
                return getDPShift(number & SHIFTSENSORNUMBERMASK);
            }
               else
               {
                   return 0;
               }
        }
}
//-------------------------------------------------------------------
void UMU::ustskBody(void)
{
    if (EnableBScan)
    {
#ifdef LARGE_BSCAN
        mScanAlarm();
#else
        read_bscan();
#endif
    }
    read_asd();

    DP_SEMAPHORE_ENTER
     for (register int ii=0; ii<QDP-1; ii++)
     {
         imitWriteDownCycleProc(ii);
//         if (pShiftSensorProc[ii]  != NULL)  pShiftSensorProc[ii]();
     }
//
#ifdef DEVICE_EMULATION
     DP_SEMAPHORE_LEAVE
#endif
//
    if (EnableBScan)
    {
        volatile register int tDiff;
        volatile register int speedCalcPeriod = TimeToTick(cSpeedCalcTime);
        tDiff = get_tickdur(speedTimer);
        if (tDiff >= speedCalcPeriod)
        {
#ifndef FAKE_V_CALCULATION
        if ((mainShiftSensorNumber & IMITATORMASK) == 0)
        { // скорость считается по основному ДП, но не имитатору
            disintpld;
            lDiff = Pathcoord[mainShiftSensorNumber] - mainShiftSensorPathcoordMemorized;
            mainShiftSensorPathcoordMemorized = Pathcoord[mainShiftSensorNumber];
            COND_IntASD_ENABLE
            DP_SEMAPHORE_LEAVE
            if (lDiff < 0)
            {
                lDiff = (lDiff ^ 0xFFFFFFFF) + 1;
            }

// коррекция, если вдруг tDiff "сильнее" больше speedCalcPeriod, чтобы скорость не выпрыгивала
            lDiff = lDiff * speedCalcPeriod / tDiff;


//                   simplePrintf("dL = %d, dT = %d", lDiff, tDiff);

//
            if (lDiff > 0xFFFF) lDiff = 0xFFFF;
        }
            else
            {
                lDiff = 0;
                DP_SEMAPHORE_LEAVE
            }
#endif
            speedTimer = xTaskGetTickCount();
            fillMessageHeader((tLANMESSAGEHEADER*)&speedMessage, speedId, idBUM, speedSz);
            speedMessage.Speed = (unsigned short)lDiff;
            speedMessage.Header.NotUse = 0;
            putmsg((unsigned char*)&speedMessage, speedSz+hdrsize);
        }
            else
            {
                DP_SEMAPHORE_LEAVE
            }
    }
        else
        {
            DP_SEMAPHORE_LEAVE
        }
/*
// отслеживание датчиков болтового стыка
    if (jointSensorDisable == FALSE)
    {
     register USHORT s;
         s = Rd_RegPLD(jointSensorAdr);
         if ((s ^ jointSensorState) & jointSensorMask)
         {
            if ((s ^ jointSensorState) & jointSensorMaskL)
            {
                if (s & jointSensorMaskL) make_jointSensor(SideLeft,TRUE);
                    else make_jointSensor(SideLeft,FALSE);
            }
           if ((s ^ jointSensorState) & jointSensorMaskR)
            {
                if (s & jointSensorMaskR) make_jointSensor(SideRight,TRUE);
                    else make_jointSensor(SideRight,FALSE);
            }
            jointSensorState = s;
         }
    }
*/
//
#ifndef AC_dis
   TakeSemaphore(s_ACTH);
   disintpld;
   switch(ACCalcStage)
   {
/*
       case ACTHCALC_CNTQUERY:
           simplePrintf("\n ACTHCALC_CNTQUERY");
           break;

       case ACTHCALC_WAITFORDATA:
           simplePrintf("\n ACTHCALC_WAITFORDATA");
           break;
*/
       case ACTHCALC_COMPUTEQUERY:
       {
//                  simplePrintf("\n ACTHCALC_COMPUTEQUERY");

           if ( (gatheringSumCnt > (cGATHERINGSUMDURATION - ACTHRARRAY_DEPTH)) || (gatheringSumCnt ==0) )
           {
               shiftACDataSums();
//                      printACDataSums(1, 1, 2);
           }
               else
               {
                   if (gatheringSumCnt == (cGATHERINGSUMDURATION - ACTHRARRAY_DEPTH))
                   {
                       sortACDataSumS();
                   }
                   addACDataSums();
//                          printACDataSums(1, 1, 2);
               }

           if (gatheringSumCnt != 0) gatheringSumCnt--;
           if (gatheringSumCnt != 0)
           {
               acThCalcPeriodCnt = cACSumGatheringPeriodByCycle;
               ACCalcStage = ACCALC_NEED_TO_START;
               break;
           }
               else
               {
                   ACCalcStage = ACTHCALC_COMPUTEEXEC;
                   break; // именно так!
               }
       }
       case ACTHCALC_COMPUTEEXEC:
       {
//                simplePrintf("\n ACTHCALC_COMPUTEEXEC");
           acThCalcPeriodCnt = ACThCalcPeriodByCycle;
           Wr_RegPLD(ACControlReg, ACControlValue_EVAL);
           ACCalcStage = ACCALC_NEED_TO_START;
           calculateAndSetTh(acThPercent);
           break;
       }
       case ACCALC_NEED_TO_START:
       {
           Wr_RegPLD(ACSumStartReg, ACSumStartValue);
           ACCalcStage = ACCALC_WAITFORDATA;
           break;
       }
   }
   COND_IntASD_ENABLE
   xSemaphoreGive(s_ACTH);
#endif
}

void UMU::PLDInterruptEmulation()
{
    if (EnableASD)  ReadASD();
    shiftValue = getShiftSensorValue(mainShiftSensorNumber);
    moveLargeBScan(0);
}

void UMU::ustsk(void *ppar)
{
    read_ascan(0);  // liniya 1
    read_ascan(1);  // liniya 2
    ustskBody();
}
