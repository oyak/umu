#include <assert.h>
#include <stdlib.h>
#include <QDebug>
#include <time.h>
#include <math.h>
#include "platforms.h"
#include "ChannelsIds.h"

#include "umudevice.h"
#include "pconst.h"
#include "MSG32041.H"
#include "MISC133.H"
#include "ustyp468.h"
#include "MTYPES4.H"

#include "MISC46_2.H"

#define __irq
#define __arm

class UMUDEVICE* UMUDEVICE::deviceObjectPtr;

#ifdef DEFCORE_OS_WIN
     class UNITWIN* UMUDEVICE::_parentClass;
     class cCriticalSection_Win* UMUDEVICE::_critical_sectionPtr;
#else
     class UNITLIN* UMUDEVICE::_parentClass;
     class cCriticalSection_Lin* UMUDEVICE::_critical_sectionPtr;
#endif

std::queue<tLAN_CDUMessage> *UMUDEVICE::_out_bufferPtr;

tLAN_CDUMessage UMUDEVICE::_BScanMessage;
unsigned int UMUDEVICE::_BScanMessageCounter;

bool *UMUDEVICE::enablePLDIntPtr;

PLDEMULATOR *UMUDEVICE::pldLPtr;
PLDEMULATOR *UMUDEVICE::pldRPtr;

// эмуляция параметров настройки БУМа, извлекаемых из файла PARAMS.INI
const unsigned int lengthPathEncoderDividerOffPar = 0;

// в проекте QT определить DEVICE_EMULATION
#define AC_dis

void dbgPrintOfMessage(tLAN_CDUMessage* _out_block)
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

static void AddToOutBuffSync(tLAN_CDUMessage* _out_block)
{
    UMUDEVICE::_critical_sectionPtr->Enter();

//    if (_out_block->Id == 0x83)
//        dbgPrintOfMessage(_out_block);

    UMUDEVICE::_out_bufferPtr->push(*_out_block);
    UMUDEVICE::_critical_sectionPtr->Release();
}
//
static void AddToOutBuffNoSync(tLAN_CDUMessage* _out_block)
{
//    if (_out_block->Id == 0x83)
//        dbgPrintOfMessage(_out_block);

    UMUDEVICE::_out_bufferPtr->push(*_out_block);
}


#define TimeToTick(time) time

int xTaskGetTickCount(void)
{
    return GetTickCount_();
}

#define __no_init
#define TBD
#define PARAM_UNDEFINED


typedef class cCriticalSection* xSemaphoreHandle;
#define xTaskHandle int

#define smprintf simplePrintf
#define simplePrintf(c, ...) // qWarning()<<c


#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/*
#define COND_IntASD_ENABLE \
{\
}
*/

#define disintpld \
{\
    *UMUDEVICE::enablePLDIntPtr = false; \
}

#define enintpld \
{\
    *UMUDEVICE::enablePLDIntPtr = true; \
}
//#define Set_CSPLD
//#define Reset_CSPLD

// задержка в единицах по 168 нС
// задавать задержки не более секунды
void delay(unsigned int value)
{
timespec t = {t.tv_sec = 0};
    t.tv_nsec = value * 168;
    nanosleep(&t, NULL);
}

void vSemaphoreCreateBinary(xSemaphoreHandle& pHandle)
{
    pHandle = UMUDEVICE::_parentClass->createCriticalSection();
}

void TakeSemaphore(xSemaphoreHandle x)
{
    UMUDEVICE::_parentClass->criticalSectionEnter(reinterpret_cast <class cCriticalSection_Lin*>(x));
}

void xSemaphoreGive(xSemaphoreHandle x)
{
    UMUDEVICE::_parentClass->criticalSectionRelease(reinterpret_cast <class cCriticalSection_Lin*>(x));
}

void vTaskDelay(unsigned int value)
{
    SLEEP(value);
}

// измерение напряжения аккумулятора
DWORD get_msrd(void)
{
    return 0;
}


void putmsg(UCHAR *srcbuf, USHORT size, vfuncpv userproc)
{
    if (userproc) userproc((void*)srcbuf);
    AddToOutBuffSync(reinterpret_cast<tLAN_CDUMessage*>(srcbuf));
}

void put_DataByWord(unsigned int address, USHORT size)
{
    assert(UMUDEVICE::pldRPtr);
    assert(UMUDEVICE::pldLPtr);
    while((size) && (UMUDEVICE::_BScanMessageCounter < UMUDEVICE::_BScanMessage.Size))
    {
        UMUDEVICE::_BScanMessage.Data[UMUDEVICE::_BScanMessageCounter++] = UMUDEVICE::pldRPtr->readRegister(address);
        UMUDEVICE::_BScanMessage.Data[UMUDEVICE::_BScanMessageCounter++] = UMUDEVICE::pldLPtr->readRegister(address);
        size -= 2;
        address += 2;
    }
}
// загрузка начиная с начала сообщения
void put_DataByWord(unsigned short *srcBuf, USHORT size)
{
unsigned char *destPtr;

    assert (!(size & 0x01) && size); // size - четное число (размер в байтах), неравное 0
    destPtr = reinterpret_cast<unsigned char*>(&UMUDEVICE::_BScanMessage);
    for(unsigned short ii=0; ii < size; ii += 2)
    {
        *destPtr++ = (unsigned char)(*srcBuf & 0xFF);
        *destPtr++ = (unsigned char)(*srcBuf >> 8);
        srcBuf++;
        if (ii >= LAN_MESSAGE_BIG_HEADER_SIZE)
        {
            UMUDEVICE::_BScanMessageCounter += 2;
        }
    }
}

#define release_Buffer \
{\
unsigned char *destPtr; \
unsigned char *srcPtr; \
    destPtr = reinterpret_cast<unsigned char*>(&UMUDEVICE::_BScanMessage); \
    srcPtr = reinterpret_cast<unsigned char*>(header); \
    for (unsigned int ii=0; ii < sizeof(header); ++ii) \
    { \
        *destPtr++ = *srcPtr++; \
     }\
    AddToOutBuffNoSync(&UMUDEVICE::_BScanMessage); \
    UMUDEVICE::_critical_sectionPtr->Release(); \
}
//

unsigned short Rd_RegPLD(unsigned int regAddr)
{
    assert(UMUDEVICE::deviceObjectPtr);
    return (UMUDEVICE::deviceObjectPtr->readPLDRegister(usLeft, regAddr) << 8) | UMUDEVICE::deviceObjectPtr->readPLDRegister(usRight, regAddr);
}

void Wr_RegPLD(unsigned int regAddr, unsigned short value)
{
    assert(UMUDEVICE::deviceObjectPtr);
    UMUDEVICE::deviceObjectPtr->writePLDRegister(usLeft, regAddr, (unsigned char)(value >> 8));
    UMUDEVICE::deviceObjectPtr->writePLDRegister(usRight, regAddr, (unsigned char)(value & 0xFF));
}

void writeIntoRAM(unsigned short address, unsigned short value)
{
    assert(UMUDEVICE::deviceObjectPtr);
    UMUDEVICE::deviceObjectPtr->writeIntoRAM(usLeft, address, (unsigned char)(value >> 8));
    UMUDEVICE::deviceObjectPtr->writeIntoRAM(usRight, address, (unsigned char)(value & 0xFF));
}

unsigned short readFromRAM(unsigned short address)
{
    assert(UMUDEVICE::deviceObjectPtr);
    return (UMUDEVICE::deviceObjectPtr->readFromRAM(usLeft, address) << 8) | UMUDEVICE::deviceObjectPtr->readFromRAM(usRight, address);
}

unsigned short get_devicenumber(void)
{
    return 1;
}

#define SUSPEND_ISR_TASK()

void get_Access(USHORT size);

#include "us465d.h"

#include "_umuvar32.cpp"
#include "_us320444.cpp"
#include "_us_32048.cpp"
#include "_us4619.cpp"


void get_Access(USHORT size)
{
    assert(size >= LAN_MESSAGE_BIG_HEADER_SIZE);
    UMUDEVICE::_critical_sectionPtr->Enter();
    UMUDEVICE::_BScanMessage.Size = size - LAN_MESSAGE_BIG_HEADER_SIZE;
    UMUDEVICE::_BScanMessageCounter = 0;
#ifndef SHORT_HEADER
    ATTACH_MESSAGE_NUMBER(UMUDEVICE::_BScanMessage.MessageCount);
#endif
}

//char CDULocalIpAddress[] = "127.0.0.1";
//char CDURemoteIpAddress[] = "127.0.0.1";

//char CDULocalIpAddress[] = "192.168.100.100";
//char CDURemoteIpAddress[] = "192.168.100.1";

//char PCLocalIpAddress[] = "192.168.100.1";
//char PCRemoteIpAddress[] = "192.168.100.3";

// parentClass - указатель на объект либо UNITLIN, либо UNITWIN
UMUDEVICE::UMUDEVICE(cThreadClassList* ThreadClassList, void *parentClass, CONFIG *pConfig): _pConfig(pConfig)
{
cDataTransferLan::cLanConnectionParams connectionParams;
cCriticalSection *pCS1;
cCriticalSection *pCS2;

    _movingDirection = Test::DirNotDefined;
#ifdef DEFCORE_OS_WIN
     _pTrolley = new TROLLEY(reinterpret_cast<UNITWIN*>(parentClass)->createCriticalSection());

#else
     _pTrolley = new TROLLEY(reinterpret_cast<UNITLIN*>(parentClass)->createCriticalSection());
#endif


#ifdef DEFCORE_OS_WIN
     pCS1 = reinterpret_cast<UNITWIN*>(parentClass)->createCriticalSection();
     pCS2 = reinterpret_cast<UNITWIN*>(parentClass)->createCriticalSection();
#else
     pCS1 = reinterpret_cast<UNITLIN*>(parentClass)->createCriticalSection();
     pCS2 = reinterpret_cast<UNITLIN*>(parentClass)->createCriticalSection();
#endif
    _pEmulator = new EMULATOR(pCS1, pCS2, TROLLEY::step, _pConfig->getPathToObjectsFiles());
    _pEmulator->getChannelList(_channelList);
//
      moveLargeBScanInit
//
     _thlist = ThreadClassList;
#ifdef DEFCORE_OS_WIN
     _parentClass = reinterpret_cast<UNITWIN*>(parentClass);
     _critical_section[CDUoutBufferIndex] = reinterpret_cast<UNITWIN*>(parentClass)->createCriticalSection();
     _critical_section[PCoutBufferIndex] = reinterpret_cast<UNITWIN*>(parentClass)->createCriticalSection();
     _critical_sectionPtr = reinterpret_cast<cCriticalSection_Win*>(_critical_section[CDUoutBufferIndex]);
#else
     _parentClass = reinterpret_cast<UNITLIN*>(parentClass);
     _critical_section[CDUoutBufferIndex] = reinterpret_cast<UNITLIN*>(parentClass)->createCriticalSection();
     _critical_section[PCoutBufferIndex] = reinterpret_cast<UNITLIN*>(parentClass)->createCriticalSection();
     _critical_sectionPtr = reinterpret_cast<cCriticalSection_Lin*>(_critical_section[CDUoutBufferIndex]);
#endif
     _out_bufferPtr = &_CDU_out_buffer;

     _enablePLDInt = false;
     enablePLDIntPtr = &_enablePLDInt;

#ifdef DEFCORE_OS_WIN
     _pldl = new PLDEMULATOR(reinterpret_cast<UNITWIN*>(parentClass)->createCriticalSection());
     _pldr = new PLDEMULATOR(reinterpret_cast<UNITWIN*>(parentClass)->createCriticalSection());
#else
     _pldl = new PLDEMULATOR(reinterpret_cast<UNITLIN*>(parentClass)->createCriticalSection());
     _pldr = new PLDEMULATOR(reinterpret_cast<UNITLIN*>(parentClass)->createCriticalSection());
#endif
     pldLPtr = _pldl;
     pldRPtr = _pldr;

     connect(_pTrolley, SIGNAL(pathStep(int, int, int)), this, SLOT(_onPathStep(int, int, int)));

     _PLDIntTimer.setInterval(1);
     connect(&_PLDIntTimer, SIGNAL(timeout()), this, SLOT(_onPLDInt()));
     _PLDIntTimer.start();
//
    _receiveStartOffset = 0;
    _write_error_count[CDUoutBufferIndex] = 0;
    _write_error_count[PCoutBufferIndex] = 0;

    setState(Stopped);
    _endWorkFlag = false;

    _dtLan = new (std::nothrow) cDataTransferLan;
// БУИ-соединение
    _CDUConnected = false;

//    strncpy(connectionParams._local_ip, CDULocalIpAddress, cDataTransferLan::cLanConnectionParams::_ip_size);
//    strncpy(connectionParams._remote_ip, CDURemoteIpAddress, cDataTransferLan::cLanConnectionParams::_ip_size);
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

    _CDUConnection_id = _dtLan->addConnection(reinterpret_cast<cDataTransferLan::cLanConnectionParams* const>(&connectionParams));
//
// ПК-соединение
    _PCConnected = false;

//    strncpy(connectionParams._local_ip, PCLocalIpAddress, cDataTransferLan::cLanConnectionParams::_ip_size);
//    strncpy(connectionParams._remote_ip, PCRemoteIpAddress, cDataTransferLan::cLanConnectionParams::_ip_size);

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
    ush_init();

#ifdef DEFCORE_OS_WIN
     _pPingTimerCS = reinterpret_cast<UNITWIN*>(parentClass)->createCriticalSection();

#else
     _pPingTimerCS = reinterpret_cast<UNITLIN*>(parentClass)->createCriticalSection();
#endif

    _needToPing = false;
     connect(&_pingTimer, SIGNAL(timeout()), this, SLOT(_onPingTimer()));
    _pingTimer.start(PING_PERIOD);


    _PCLinkFault = false;
     connect(&_PCLinkFaultTimer, SIGNAL(timeout()), this, SLOT(_onPCLinkFaultTimer()));
    _PCLinkFaultTimer.setInterval(PC_LINK_FAULT_TIMEOUT);
    _PCLinkFaultTimer.start();
     connect(this, SIGNAL(restartPCLinkFaultTimer()), this, SLOT(_onRestartPCLinkFaultTimer()));


    _engineThreadIndex = _thlist->AddTick(DEFCORE_THREAD_FUNCTION(UMUDEVICE, this, engine));
    _thlist->Resume(_thlist->AddTick(DEFCORE_THREAD_FUNCTION(UMUDEVICE, this, Tick)));
}
//
UMUDEVICE::~UMUDEVICE()
{
    deviceObjectPtr = nullptr;
    delete _pldl;
    delete _pldr;
    delete _pTrolley;
}

void UMUDEVICE::start(void)
{
    _thlist->Resume(_engineThreadIndex);
}

void UMUDEVICE::stop(void)
{
    setState(Finishing);
    _endWorkFlag = true;
    SLEEP(2000);
}

bool UMUDEVICE::engine(void)
{
    SLEEP(1);
    switch (_state)
    {
        case Stopped:
#ifndef SKIP_CDU_CONNECTING
        if (_dtLan->openConnection(_CDUConnection_id) == 0)
        {
            _CDUConnected = true;
            setState(CDUConnected);
        }
            else
            {
                SLEEP(500);
            }
#else
        setState(CDUConnected);
#endif
        break;
//
        case CDUConnected:
//            emit CDUconnected();
        if (_CDUConnected)
         {
            _thlist->Resume(_thlist->AddTick(DEFCORE_THREAD_FUNCTION(UMUDEVICE, this, umuTick)));
        }
        setState(PCConnecting);
        break;
//
       case PCConnecting:
        _PCConnected = false;
#ifndef SKIP_PC_CONNECTING
        if (_dtLan->openConnection(_PCConnection_id) == 0)
        {
            _PCLinkFault = false;
            _PCConnected = true;
            setState(WhenConnected);
        }
            else
            {
                SLEEP(500);
            }
#else
        setState(WhenConnected);
#endif
        break;

//
        case WhenConnected:
            setState(Working);
            break;
//
        case Working:
#ifndef SKIP_PC_CONNECTING
        if ((_pConfig->getRestorePCConnectionFlagState()) && (_PCLinkFault))
        {
            _pTrolley->stopTrolley();
            _dtLan->closeConnection(_PCConnection_id);
            setState(PCConnecting);
        }
#endif
        break;

        case Finishing:
        break;

        default: assert(0);
    }
    return !_endWorkFlag;
}

UMUDEVICE::eState UMUDEVICE::getState(void)
{
    return _state;
}

void UMUDEVICE::setState(UMUDEVICE::eState newState)
{
    _state = newState;
}

bool UMUDEVICE::isEndWork()
{
    return (_state == Finishing);
}
//
bool UMUDEVICE::Tick()
{
    _pPingTimerCS->Enter();
    if ((_PCConnected) && (_needToPing))
    {
        sendPingToPC();
        _needToPing = false;
    }
    _pPingTimerCS->Release();

    TickSend();
    if (_CDUConnected) this->TickCDUReceive();
    if (_PCConnected) this->TickPCReceive();

    SLEEP(1);
    return !_endWorkFlag;
}
//
void UMUDEVICE::TickSend()
{
    if (_CDUConnected) unload(CDUoutBufferIndex);
    if (_PCConnected) unload(PCoutBufferIndex);
}
//
void UMUDEVICE::unload(eOutBufferIndex outBufferIndex)
{
bool res;
    assert((outBufferIndex == CDUoutBufferIndex) || (outBufferIndex == PCoutBufferIndex));
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
        _receiveStartOffset = lanmsgparcer(_incmdbuf, (unsigned short)(res + _receiveStartOffset));
    }
}
//
void UMUDEVICE::TickPCReceive()
{
bool fRepeat = false;
unsigned int res = 0;

    do {
        fRepeat = false;
        switch (_read_state) {
        case rsHead: {
            readPCMessageHead(res, fRepeat);
            break;
        }
        case rsBody: {
            readPCMessageBody(res, fRepeat);
            break;
        }
        case rsOff:
            break;
        }
    } while (fRepeat);
}
//
bool UMUDEVICE::umuTick()
{
    ustsk(0);
    SLEEP(1);
    return !isEndWork();
}

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




void UMUDEVICE::readPCMessageHead(unsigned int& res, bool& fRepeat)
{
    _currentMessage.resetMessage();
    res = _dtLan->read(_PCConnection_id, reinterpret_cast<unsigned char*>(&_currentMessage), LAN_MESSAGE_SHORT_HEADER_SIZE);

    if (res == LAN_MESSAGE_SHORT_HEADER_SIZE) {
        fRepeat = true;
        _read_state = rsBody;
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

void UMUDEVICE::readPCMessageBody(unsigned int& res, bool& fRepeat)
{
    DEFCORE_ASSERT(_read_bytes_count <= tLAN_PCMessage::LanDataMaxSize);
    res = _dtLan->read(_PCConnection_id, reinterpret_cast<unsigned char*>(&_currentMessage.Data), _read_bytes_count);

    if (res == _read_bytes_count) {
        fRepeat = true;
        _read_state = rsHead;
        unPack(_currentMessage);
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

            _pEmulator->deletePathObjects();
//
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
                assert(0);
            }
            _pEmulator->setMovingDirection(_movingDirection);
            _pTrolley->setMovingDirection(_movingDirection);

            bytePtr += sizeof(int32_t);

            byteCount = static_cast<int>(buff.Size - sizeof(int32_t));
            for (unsigned int jj=0; jj < 2; ++jj)
            {
                if (byteCount == 0) break;
                IdCount = ReadLE16U (bytePtr);
                if (byteCount < IdCount * (sizeof(coord) + sizeof(id) )) break;

                if (jj == 0) qWarning() << "left side flaw count = " << IdCount;
                    else qWarning() << "right side flaw count = " << IdCount;

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
                        if (res) qWarning() << "left side flaw: id = " << id << ", coord = " << coord;
                            else qWarning() << "left side flaw: id = " << id << ", coord = " << coord << " - ignored";
                    }
                       else
                       {
                           bool res;
                           res = _pEmulator->onMessageId(id, coord, usRight);
                           if (res) qWarning() << "right side flaw: id = " << id << ", coord = " << coord;
                               else qWarning() << "right side flaw: id = " << id << ", coord = " << coord << " - ignored";
                       }

                }
                byteCount -= IdCount * (sizeof(coord) + sizeof(id));
            }
            qWarning() << "TrackMapId";
            break;
        }
//
        case NextTrackCoordinateId:
        {
            tNEXTTRACKCOORD *pMessage;
            if (buff.Size == sizeof(tNEXTTRACKCOORD))
            {
                pMessage = reinterpret_cast<tNEXTTRACKCOORD*>(buff.Data);
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
              _pTrolley->setCoordinate(pMessage->Coord, pMessage->LeftDebugCoord, pMessage->RightDebugCoord);
              qWarning() << "JumpTrackCoordinateId";
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
        if (EnableASD)  ReadASD();
        shiftValue = getShiftSensorValue(mainShiftSensorNumber);
        moveLargeBScan(0);
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
            strokeAndLine = _pEmulator->CIDToLineAndStroke(*it);
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
                     strokeAndLine = _pEmulator->CIDToLineAndStroke(*it);
                     _pldl->resetSignals(strokeAndLine.Stroke, strokeAndLine.Line);
                 }
             }
//
    pSignalsData = _pEmulator->getScanObject(usRight, coordRInMM, isDataObject);
    if (pSignalsData)
    {
        for (it=_channelList.begin(); it != _channelList.end(); ++it)
        {
            strokeAndLine = _pEmulator->CIDToLineAndStroke(*it);
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
                 strokeAndLine = _pEmulator->CIDToLineAndStroke(*it);
                 _pldr->resetSignals(strokeAndLine.Stroke, strokeAndLine.Line);
             }
        }
}

unsigned int UMUDEVICE::getNumberOfTacts()
{
    return _pldl->getNumberOfTacts();
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
