#ifndef UMUDEVICE_H
#define UMUDEVICE_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <queue>
#include <QDebug>
#include <QString>
#include <QStringList>
#include "platforms.h"
#include "unitlin.h"

#include "datatransfer_lan.h"
#include "sockets/socket_lan.h"
#include "CriticalSection.h"
#include "ThreadClassList.h"
#include "IPOPT43.H"
#include "pldemu.h"
#include "trolley.h"
#include "emulator.h"
#include "config.h"
#include "logfile.h"

#include "pconst.h"
#include "defsubst.h"
#include "MISC46_2.H"
#include "ustyp468.h"

//define SKIP_CDU_CONNECTING
//define SKIP_PC_CONNECTING

//define PATH_MAP_LOGFILE_ON // �������� ������ ���� �� ����� ����
//define TESTING_PATH_MAP_ON // �������� �������� ���������� ������� ����� ��������� ����

//define PCLAN_MESSAGE_LOGFILE_ON // �������� ������ ���� LAN-��������� �� ���������

#ifdef SKIP_CDU_CONNECTING
#ifdef SKIP_PC_CONNECTING
#error "������: � ������� ��� ethernet-���������� �������������"
#endif
#endif

class UMU;

#define N0EMS_SENSOR_SHIFT_mm 90  // �������� ��� 0�� � ����������� ������� ������������ ��
// ������ (����� ��1 � ��2), ��

#define cReceiveStartOffsetMax 0x35  // ����� ������� ��������� ��������� ��� ��� ����� 1

#define LAN_MESSAGE_SHORT_HEADER_SIZE 4
#define LAN_MESSAGE_BIG_HEADER_SIZE 6

// �������������� ��������� ������ ���-��������
enum MessageId  //: unsigned char
{
    ChangeCduModeId = 1,            //����� ������ CDU - �������
    ChangeRcModeId = 2,             //����� ������ RC � ����� (��������)
    RegistrationOnId = 3,           //�������� �����������
    RegistrationOffId = 4,          //��������� �����������
    AnswerToQuestionId = 5,         //����� �� ������
    BoltJointOnId = 6,              //������ �������� ���� ������
    BoltJointOffId = 7,             //������ �������� ���� ��������
    OperatorTrackCoordinateId = 8,  //���������� �������� ���������� (�� ��)
    OperatorActionId = 9,           //�������� ���������
    DefectMarkId = 10,              //������� � �������
    RailroadSwitchMarkId = 11,      //������� � �������
    TrackMapId = 12,                //������� ����
    NextTrackCoordinateId = 13,     //��������� ����������
    JumpTrackCoordinateId = 14,     //������ �� ���������� (������)
    ManipulatorStateId = 15,        //��������� ������������ (��������)
    PingId = 16                     //�������� ����������
};

#pragma pack(push, 1)

typedef struct _NEXT_TRACK_COORD  //
{
    int32_t Coord;
    int32_t LeftDebugCoord;
    int32_t RightDebugCoord;
    float Speed;
    uint32_t Time;
} tNEXTTRACKCOORD;

typedef struct _JUMP_TRACK_COORD  //
{
    int32_t Coord;
    int32_t LeftDebugCoord;
    int32_t RightDebugCoord;
    uint32_t Time;
} tJUMPTRACKCOORD;

typedef struct _OBJECT_DATA
{
    int32_t StartCoordinate;
    unsigned short Id;
} tOBJECTDATA;


struct tLAN_PCMessage
{
    enum eCONSTANTS
    {
        // PC
        LanDataMaxSize = 65536,
    };

    unsigned char Id;
    unsigned char Reserved;
    unsigned short Size;
    unsigned char Data[LanDataMaxSize];

    tLAN_PCMessage()
        : Id(0)
        , Reserved(0)
        , Size(0)
    {
        memset(Data, 0, LanDataMaxSize);
    }

    /*
        tLAN_PCMessage(unsigned char id, unsigned short size)
            : Id(id)
            , Source(CDU)
            , Size(size)
        {
            memset(Data, 0, LanDataMaxSize);
        }
    */
    void resetMessage()
    {
        Id = 0;
        Reserved = 0;
        Size = 0;
        memset(Data, 0, LanDataMaxSize);
    }

   QString printHeader()
   {
       return QString::asprintf("header: 0x%x 0x%x 0x%x", Id, Size & 0xFF, Size >> 8);
   }

    QStringList printBody()
    {
     QStringList res;
        if (Size)
        {
            res.append("body:");
            for(unsigned int ii=0; ii < Size; )
            {
                if ((Size - ii) >= 8) {
                    res.append(QString::asprintf("0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", Data[ii+0], Data[ii+1], Data[ii+2], Data[ii+3], Data[ii+4], Data[ii+5], Data[ii+6], Data[ii+7]));
                    ii += 8;
                }
                    else {
                        switch((Size-ii))
                        {
                            case 1:
                            {
                                res.append(QString::asprintf("0x%x", Data[ii]));
                                ii += 1;
                                break;
                            }
                            case 2:
                            {
                                res.append(QString::asprintf("0x%x 0x%x", Data[ii+0], Data[ii+1]));
                                ii += 2;
                                break;
                            }
                            case 3:
                            {
                                res.append(QString::asprintf("0x%x, 0x%x, 0x%x", Data[ii+0], Data[ii+1], Data[ii+2]));
                                ii += 3;
                                break;
                            }
                            case 4:
                            {
                                res.append(QString::asprintf("0x%x, 0x%x, 0x%x, 0x%x", Data[ii+0], Data[ii+1], Data[ii+2], Data[ii+3]));
                                ii += 4;
                                break;
                            }
                            case 5:
                            {
                                res.append(QString::asprintf("0x%x, 0x%x, 0x%x, 0x%x, 0x%x", Data[ii+0], Data[ii+1], Data[ii+2], Data[ii+3], Data[ii+4]));
                                ii += 5;
                                break;
                            }
                            case 6:
                            {
                                res.append(QString::asprintf("0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", Data[ii+0], Data[ii+1], Data[ii+2], Data[ii+3], Data[ii+4], Data[ii+5]));
                                ii += 6;
                                break;
                            }
                            case 7:
                            {
                                res.append(QString::asprintf("0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", Data[ii+0], Data[ii+1], Data[ii+2], Data[ii+3], Data[ii+4], Data[ii+5], Data[ii+6]));
                                ii += 7;
                                break;
                            }
                            default: break;
                        }
                }
            }
       }
       return res;
    }
    void copy(tLAN_PCMessage& sourse)
    {
        resetMessage();
        Id = sourse.Id;
        Size = sourse.Size;
        memcpy(Data, sourse.Data, Size);
    }

    bool messageCorrectness()
    {
     bool res = false;
        switch(Id)
        {
            case PingId:
            {
                if (Size == 0) res = true;
                break;
            }
            case NextTrackCoordinateId:
            {
                if (Size == sizeof (tNEXTTRACKCOORD)) res = true;
                break;
            }
            case JumpTrackCoordinateId:
            {
                if (Size == sizeof (tJUMPTRACKCOORD)) res = true;
                break;
            }
            case TrackMapId:
            {
                res = true;
                break;
            }
        }
        return res;
    }
};
//
struct tLAN_CDUMessage
{
    enum eCONSTANTS
    {
        // CDU
        LanDataMaxSize = 1019,
    };

    unsigned char Id;
    unsigned char Source;
    unsigned short Size;
    unsigned char MessageCount;
    unsigned char NotUse;
    unsigned char Data[LanDataMaxSize];

    tLAN_CDUMessage()
        : Id(0)
        , Source(0)
        , Size(0)
        , MessageCount(0)
        , NotUse(0)
    {
        memset(Data, 0, LanDataMaxSize);
    }

    /*
        tLAN_Message(unsigned char id, unsigned short size)
            : Id(id)
            , Source(CDU)
            , Size(size)
            , MessageCount(0)
            , NotUse(0)
        {
            memset(Data, 0, LanDataMaxSize);
        }
    */
    void resetMessage()
    {
        Id = 0;
        Source = 0;
        Size = 0;
        MessageCount = 0;
        NotUse = 0;
        memset(Data, 0, LanDataMaxSize);
    }
};

#pragma pack(pop)




#define PING_PERIOD 500             // ��
#define PC_LINK_FAULT_TIMEOUT 3000  // ��

//
class UMUDEVICE : public QObject
{
    Q_OBJECT

    enum eOutBufferIndex
    {
        CDUoutBufferIndex = 0,
        PCoutBufferIndex = 1,
        NumOfOutBuffers = 2  // ����� ����� �������
    };

public:
    static UMUDEVICE* deviceObjectPtr;

//    static std::queue<tLAN_CDUMessage>* _out_bufferPtr;  // ��������� �� ����� �������� ������ ��� ���
//    static bool* enablePLDIntPtr;  // ��������� �� ���� "����������" �� ���� ���������
//    static PLDEMULATOR* pldLPtr;
//    static PLDEMULATOR* pldRPtr;


    static char localIpAddress[];
    static char remoteIpAddress[];

    UMUDEVICE(cThreadClassList* ThreadClassList, void* parentClass, CONFIG* pConfig);
    ~UMUDEVICE();

    void start();

    void stop();

    bool engine(void);

    void writePLDRegister(eUMUSide side, unsigned int regAddress, unsigned char value);
    unsigned char readPLDRegister(eUMUSide side, unsigned int regAddress);

    void writeIntoRAM(eUMUSide side, unsigned int regAddress, unsigned char value);
    unsigned char readFromRAM(eUMUSide side, unsigned int regAddress);

    unsigned int getNumberOfTacts();

    void printConnectionStatus();
    void setPLDInterruptEnable(bool enabled);
    cCriticalSection *createCriticalSection();
    void criticalSectionEnter(cCriticalSection *pCS);
    void criticalSectionRelease(cCriticalSection *pCS);

    // ������ � �����������
    QString& getCDULocalIPAddress();
    QString& getCDURemoteIPAddress();
    bool setCDULocalIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0);
    bool setCDURemoteIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0);
    QString& getPCLocalIPAddress();
    QString& getPCRemoteIPAddress();
    bool setPCLocalIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0);
    bool setPCRemoteIPAddress(QString& IPAddressPart3, QString& IPAddressPart2, QString& IPAddressPart1, QString& IPAddressPart0);

    bool setCDULocalPort(QString port);
    bool setCDURemotePort(QString port);
    bool setPCLocalPort(QString port);
    bool setPCRemotePort(QString port);

    unsigned short getCDULocalPort();
    unsigned short getCDURemotePort();
    unsigned short getPCLocalPort();
    unsigned short getPCRemotePort();

    bool getRestorePCConnectionFlagState();
    void setRestorePCConnectionFlag(bool state);

    QString getPathToObjectsFiles();
    void setPathToObjectsFiles(QString path);

    bool testPassword(QString& password);
    void save();

    void restartCDUConection();


    void NonSyncAddToOutBuffer(tLAN_CDUMessage* _out_block);
    void NonSyncAddToOutBufferLock();
    void NonSyncAddToOutBufferUnlock();

#ifdef _test_message_numeration_integrity
    void testMessageNumerationIntegrity(tLAN_CDUMessage* _out_block);
#endif
    //
signals:
    void CDUconnected();
    void restartPCLinkFaultTimer();
    void message(QString s);
    void messageHandlerSignal(QString s); // ����������� � messageHandler()

public slots:
    void _onPLDInt();                                             // ������������ ������� _PLDIntTimer
    void _onPathStep(int shift, int coordLInMM, int coordRInMM);  // ���� �� ������ � ������������ �� �� trolley
    void _onPingTimer();
    void _onPCLinkFaultTimer();         // ���� �� ������������ _PCLinkFaultTimer
    void _onRestartPCLinkFaultTimer();  // ������������� _PCLinkFaultTimer, ����� �� ��������
    void onMessage(QString s); // ���� �� ������� � ���������� ����������� �� ������������ �������
    void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg); // ���������� ��������� �� QDebug

private:
    class UNITLIN* _parentClass;
    bool _endWorkFlag;
    bool _restartCDUConnectionFlag;
    int _PCConnection_id;   // ������������� ���������� � cDatatr c �� ���������
    int _CDUConnection_id;  // ������������� ���������� � ���

    bool _CDUConnected;  // ���������� � ��� �����������
    bool _PCConnected;   // ���������� � �� �����������

    bool _PCLinkFault;  // �������������, ���� ��� PingId �� �� � ������� PC_LINK_FAULT_TIMEOUT
    QTimer _PCLinkFaultTimer;

    unsigned int _write_error_count[NumOfOutBuffers];

    std::queue<tLAN_CDUMessage> _CDU_out_buffer;
    std::queue<tLAN_PCMessage> _PC_out_buffer;

    unsigned char _receiveStartOffset;
    unsigned char _incmdbuf[UIP_BUFSIZE + 4 + cReceiveStartOffsetMax];

    cDataTransferLan* _dtLan;

    cCriticalSection* _critical_section[NumOfOutBuffers];
    cThreadClassList* _thlist;
    int _engineThreadIndex;
    int _UmuTickThreadIndex;

    QTimer _PLDIntTimer;

    PLDEMULATOR* _pldl;
    PLDEMULATOR* _pldr;
    bool _enablePLDInt;  // ���� "����������" �� ���� ���������
    
    UMU *_umu;

    TROLLEY* _pTrolley;
    EMULATOR* _pEmulator;
    QList<CID> _channelList;

    Test::eMovingDir _movingDirection;  // ����������� ��������:

    QTimer _pingTimer;
    cCriticalSection* _pPingTimerCS;
    bool _needToPing;
    CONFIG* _pConfig;

    LOGFILE *_pPathMapLogFile;
    LOGFILE *_pLANPCMessageLogFile;

    void unload(eOutBufferIndex outBufferIndex);
    bool CDUTick(void);
    bool PCTick(void);
    void CDUTickSend();
    void PCTickSend();
    void TickCDUReceive();
    void TickPCReceive();
    bool umuTick();

//protected:
    enum eReadState  // ��������� �������� ������ ���������
    {
        rsOff = 0,   // ����������
        rsHead = 1,  // ������� ��������� ���������
        rsBody = 2,  // ������� ���� ���������
        rsTestHead = 3, // �������� ���������
        rsSkipWrongId = 4, // ������������ ���������������� �������������
    };

    tLAN_PCMessage _currentMessage;
    eReadState _read_state;
    unsigned int _read_bytes_count;
    unsigned int _error_message_count;
//
#ifdef _test_message_numeration_integrity
    unsigned char _messageNumber;
    unsigned char _lastMessageID;
#endif

    void readPCMessageHead(bool& done);
    void readPCMessageBody(bool& done);
    unsigned char skipWrongMessageId(bool &done);
    void unPack(tLAN_PCMessage& buf);
    //
    void AddToOutBuffSync(tLAN_PCMessage* _out_block);
    void AddToOutBuffNoSync(tLAN_PCMessage* _out_block);
    void sendPingToPC();

    void whenTrolleyCoordChanged(int coordLInMM, int coordRInMM);
};
//
typedef void (UMU::* vfuncv)(void);

#define enintpld {PLDInterruptEnable();}
#define disintpld {PLDInterruptDisable();}
#define SUSPEND_ISR_TASK()
#define release_Buffer \
{ \
    releaseBuffer();\
}
#define WHEN_START_PLD \
{\
    whenStartPLD();\
}
#define ATTACH_MESSAGE_NUMBER(a) \
{\
    a = attachMessageNumber();\
}
#define COND_IntASD_ENABLE \
{\
    condIntASDEnable();\
}

#define COND_INT_ASD_ENABLE_LOCK \
{\
    condIntASDLock();\
}

#define COND_INT_ASD_ENABLE_UNLOCK \
{\
    condIntASDUnlock();\
}

#define DP_SEMAPHORE_ENTER \
{\
disintpld \
TakeSemaphore(s_ImitDP);\
}

#define DP_SEMAPHORE_LEAVE \
{\
xSemaphoreGive(s_ImitDP);\
COND_IntASD_ENABLE \
}

#define REDEFINE_DP_CYCLE_PROC \
{\
DP_SEMAPHORE_ENTER \
redefineDPCycleProc(); \
DP_SEMAPHORE_LEAVE\
}

class UMU
{
public:
    static const unsigned char mask[8];
    static const unsigned char mask2[8];
    static int xTaskGetTickCount(void);

    UMU(UMUDEVICE* parent);
    ~UMU(){};

    void PLDInterruptEmulation();
    void ustsk(void *ppar);
    void moveLargeBScanInit();
    void ush_init(void);
    tres lanmsgparcer(UCHAR* buf, USHORT lng);
    void dbgPrintOfMessage(tLAN_CDUMessage* _out_block);

private:

    UMUDEVICE* _device;
    unsigned short _BScanMessageHeader[4];
    tLAN_CDUMessage _BScanMessage;
    unsigned int _BScanMessageCounter;

    tSPEEDMESSAGE speedMessage;
    tDPCOORDMESSAGEEX DPMessage;
//
    USHORT deviceSerialNumber;

    unsigned char tempbuf[tempbuf_size];
    unsigned char O1Data[o1data_size];
    unsigned char NumOfTacts;  // number of tacts - for 2 sides and lines

    DWORD AscanSendTick[2];  // [0] - liniya 1, [1] - liniya 2

    // format ascan[]...ascanstrobformax[]
    // element 0 - liniya 1, lev. storona
    // element 1 - liniya 2, lev. storona
    // element  2 - liniya 1, prav. storona
    // element  3 - liniya 2, prav. storona
    // if ascan[] == 0 ascan is switch off
    //
    //   ascan[]   2^x values
    //      0   - otkl
    //      1   - zapis parametrov v registry
    //      2   - ozhidanie gotovnosti
    //      4   - est gotovnost dannyh
    //      8   - tekushii takt okonchen

    unsigned char ascan[4];
    unsigned char curchan[4];
    unsigned char ascanscale[4];
    unsigned char ascanstart[4];
    unsigned char ascanregim[4];
    unsigned char ascanstrobformax[4];
    xSemaphoreHandle s_ascan,s_asd;

    unsigned char EnableASD;
    DWORD ASDSendTick;

    unsigned short ASDtype[tactbitmsk+1];

    unsigned short ASD_PreBuf[tactbitmsk+1][5];
    unsigned short ASD_Buffer[tactbitmsk+1];

    // ������ �.�. ���������� � �����
    vfuncv pShiftSensorProc[QDP]; // ������� ��������������� (mainShiftSensorNumber & SHIFTSENSORNUMBERMASK) ����� NULL � �� ������������
    // ����� ��� � ��������� �������

    unsigned char mainShiftSensorNumber; // ���������� ����� �� ��� ��������� - ������� "��������" - � ������� IMITATORMASK

    unsigned char  ImitDPOn[QDP]; // ���� 0, �������� ��-x ��������, 1,2... - ���������������� � ustsk() ������  imitDPIncTime[]
    // ����� �������� ������ IMITDPWAIT, �������� ��������� 3D

    // ��� ��������� ������� ��2 � ������ IMITDPSPECMODE1 + �������� ��
    // �� ���� ���� (���������) ��2 ����������:
    //  1)  A - ��� ���������� ��������� ��������� ��
    //  2)  2*A - ��� ���������� ��������� ��������� ��
    //  3)  A - ��� ���������� ��������� ��������� ��
    unsigned char parameterA;     // �������� ������ ��������� ��
    unsigned char paramACounter;
    unsigned char parameterB;   // ���������� ������ ��2
    unsigned char scanerImitStage; // ��. SCANINITSTAGES

    enum SCANIMITSTAGES
    {
        SCANIMITINIT = 0,
        SCANIMITINC1 = 1,
        SCANIMITDEC = 2,
        SCANIMITINC2 = 3
    };

    unsigned char imitDPStep[QDP]; // ��. IMITATIONDPSTEP

    DWORD imitDPIncTime[QDP];


    xSemaphoreHandle s_ImitDP;  //
    DWORD imitDPIncTick[QDP];

    int   Pathcoord[QDP*2];  // full coordinate of DP (signed!)
    DWORD speedTimer;   // ��� ������� �������� - ����� �������� ��������� �� �� ����� cSpeedCalcTime
    // ��� �������� ��������� ���������� �������� B-���������
    int mainShiftSensorPathcoordMemorized;

    unsigned int displayCoord; // ���������� ���������� ��� ��������� �� (��� ��������� ��������� ���������� ���������� �� �����)
    // ������������ ��� ������ ��������� �� ��� ��������� ��� ��������
    unsigned char fDPCoordDataExtended; // 0/1 - 1 - ���������� ������ � ������������ ��������� �� � ����������� ������� (� ����������
    //����������� ��. �.4.31) ���� ���������������/������������ � ������� ������� ���������� ������� ��� (�.4.7 ���������)

    xTaskHandle h_moveLargeBScan;
    unsigned char  shiftValue; // �� ������
    unsigned char fResetMScanTimer;

    unsigned char EnableBScan;
    // fMScan - ��������������� ������ �������� MScanSendTime, ���� ��� �������� ������.
    // ���� (fMScan == 1) � ��� ��������, �� ���������� ������ ��������� �� � ������� ����������� ����������.
    unsigned char fMScan;
    DWORD MScanSendTick;

    USHORT MaxAmp,MaxDelay1,MaxDelay2;

    unsigned char flWasStopped;
    DWORD ascancounter;
    unsigned char fIntPLDMustBeEnable;
    unsigned char messageNumber;

    DWORD versionInfoSendTick;
    DWORD versionInfoTimeout;

    BOOL fIsTrolleyDP;        // ������� FALSE/TRUE - ��������� ������ ������� ��/ ������ dL �������

    volatile int lDiff; // ������ ������� ����� ����������� � ������� ��������� ��������� �� �� ������ cSpeedCalcTime

//    const unsigned char mask[8];
//    const unsigned char mask2[8];

    void PLDInterruptEnable();
    void PLDInterruptDisable();
    void delay(unsigned int value);
    void vSemaphoreCreateBinary(xSemaphoreHandle& pHandle);
    void TakeSemaphore(xSemaphoreHandle x);
    void xSemaphoreGive(xSemaphoreHandle x);
    void vTaskDelay(unsigned int value);
    DWORD get_msrd(void);
    void putmsg(UCHAR *srcbuf, USHORT size);
    void get_Access(unsigned short size);
    void releaseBuffer();
    void put_DataByWord(unsigned int address, USHORT size);
    void put_DataByWord(unsigned short *srcBuf, USHORT size);     // �������� ������� � ������ ���������

    unsigned short Rd_RegPLD(unsigned int regAddr);
    void Wr_RegPLD(unsigned int regAddr, unsigned short value);
    void writeIntoRAM(unsigned short address, unsigned short value);
    unsigned short readFromRAM(unsigned short address);
    unsigned short get_devicenumber(void){ return 1;}

    void intdisf(){};
    void intenf(){};
    void whenStartPLD();
    void attachMessageNumber(void* par)
    {
       ((unsigned char*)par)[4] = messageNumber;
       messageNumber++;
    }
    unsigned char attachMessageNumber()
    {
        unsigned char a = messageNumber;
        messageNumber++;
        return a;
    }
    void attachMessageNumberToHeader(tLANMESSAGEHEADER *pHdr)
    {
       pHdr->MessageCount = messageNumber;
       messageNumber++;
    }
    //-------------------------------------------------------------------
    void attachMessageLengthToHeader(tLANMESSAGEHEADER *pHdr,unsigned short len)
    {
       pHdr->Size = len;
    }
    unsigned char GET_DPx_VALUE(unsigned char DPNumber)
    {
        if (DPNumber == 0) return (Rd_RegPLD(LENGTHDPVALUEADR) >> 8);
            else return 0;
    }
    unsigned char getSideIndex(unsigned char *pByte)
    {
       return  *pByte >> 7;
    }
    //-------------------------------------------------------------------
    unsigned char getLineIndex(unsigned char *pByte)
    {
      return (*pByte & linebitmsk) >> 6;
    }
    //-------------------------------------------------------------------
    unsigned char getTacktNumber(unsigned char *pByte)
    {
      return *pByte &  tactbitmsk;
    }
    //-------------------------------------------------------------------
    void condIntASDEnable()
    {
       if (fIntPLDMustBeEnable) enintpld;
    }
    void restartAScan(void)
    {
    register unsigned char i;
        for (i = 0; i < 4; ++i)
        {
          if (ascan[i] > 1) ascan[i] = 1;
        }
    }
    void printTactParams(unsigned char tact){};

    void read_ascan(UCHAR line);
    void Get_Uakk(void);
    void PLDEn(void){};
    void PDLDis(void){};
    void ReloadBUM(void){};
    void SetNumOfTacts(UCHAR *ptr);
    BOOL isAScanStopped(BOOL fUnlocked);
    void StopAScan(void);
    void StartBScan(void);
    void StopBScan(void);
    void Stop_PLD_Work(void);
    void Start_PLD_Work(void);
    void StartPLD(void);
    unsigned char correctDACValue(unsigned char value);
    void SetTacktParam(tTACTPARAMSCMD *pData);
    void ChangeVRU(UCHAR *p);
    void ChangeStrobs(UCHAR *p);
    void Change2Tp(unsigned char *p);
    void setBScanSignalsCutStartCmd(tBSCANSIGNALSCUTSTARTCMD * pData);
    void setExendedFormatOfDPValMessageCmd(BOOL state);
    void ReadASD(void);
    void read_asd(void);
    void setImitDPStep(unsigned char _imitDpStepValue, unsigned char DPNum );
    void fillDPMessageHdr(void *pDPMessage, BOOL extendedFormat);
    void fillMessageByDPStepData(unsigned char DPNum, void *pDPMessage, unsigned char DPData, BOOL extendedFormat);
    void moveLargeBScanBody(unsigned short *pHeader);
    void moveLargeBScan(void *ppar);
    void initMScanFlags(void);
    void mScanAlarm(void);
    void setRelayCtrlSignals(unsigned char command);
    void setDPOption(unsigned short value);
    void setTrolleyDP();
    void setScanerDP();
    void KSwitch(UCHAR fScanerOn);
    void scanerSwitch(UCHAR *p);
//
    tres parcer(UCHAR* pmsg,USHORT dataLength);
    void varsinit(void);
    void BUMctrlproc(UCHAR *ptr);
    void ASD_On(void);
    void ASD_Off(void);
    void make_C3(void);
    void SendVersionInfo(void);
    void StartAScan(UCHAR *ptr);
    void SetDPval(UCHAR *p);
    void setDPImitCmd(UCHAR *p);
    void doWhenImitSwitchOFF(unsigned char relativeNum);
    void doWhenScanerImitSwitchOFF(void);
    void setImit(unsigned char *p,unsigned char fExtendedCmd);
    void setDPImitExCmd(UCHAR *p);
    void setScanImitCmd(UCHAR *p);
    void make_jointSensor(UCHAR side,BOOL newState);
    void imitWriteDownCycleProc(UCHAR imitNumber);
    void pathEncoderCycleProc(unsigned char relativeNumber);
    void trolleyPathCycleProc(void);
    void scanerLCycleProc(void);
    void scanerHCycleProc(void);
    void imitCycleProc(unsigned char relativeNumber);
    void imitTrolleyPathCycleProc(void);
    void imitScanerLCycleProc(void);
    void imitScanerHCycleProc(void);
    void redefineDPCycleProc(void);
    void scanerImitProc(void);
    void sendDPData(unsigned char sensorNum, unsigned char sensorData);
    unsigned char getDPShift(unsigned char relativeNumber);
    unsigned char getImitShift(unsigned char relativeNumber);
    unsigned char getShiftSensorValue(unsigned char number);
    void ustskBody(void);
    void condIntASDLock(){fIntPLDMustBeEnable = 0;}
    void condIntASDUnlock(){fIntPLDMustBeEnable = 1;}
};

#endif
