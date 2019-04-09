#ifndef UMUDEVICE_H
#define UMUDEVICE_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <queue>
#include "platforms.h"
#ifdef DEFCORE_OS_WIN
#include "unitwin.h"
#else
#include "unitlin.h"
#endif

#include "datatransfer_lan.h"
#include "sockets/socket_lan.h"
#include "CriticalSection.h"
#include "ThreadClassList.h"
#include "IPOPT43.H"
#include "pldemu.h"
#include "trolley.h"
#include "emulator.h"

//#define SKIP_CDU_CONNECTING
//#define SKIP_PC_CONNECTING

#ifdef SKIP_CDU_CONNECTING
#ifdef SKIP_PC_CONNECTING
#error "������: � ������� ��� ethernet-���������� �������������"
#endif
#endif



#define  cReceiveStartOffsetMax 0x35 // ����� ������� ��������� ��������� ��� ��� ����� 1



#define LAN_MESSAGE_SHORT_HEADER_SIZE 4
#define LAN_MESSAGE_BIG_HEADER_SIZE 6


#pragma pack(push, 1)
struct tLAN_PCMessage
{
    enum eCONSTANTS
    {
// PC
        LanDataMaxSize = 1500,
        LANBufferSize = 2048
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
};
//
struct tLAN_CDUMessage
{
    enum eCONSTANTS
    {
// CDU
        LanDataMaxSize = 1019,
        LANBufferSize = 1024
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

/*
// ��������� ��������� � ������ �� ����
typedef struct _MESSAGEHEADER
{
    unsigned char Id;    // eMESSAGEID
    unsigned char Reserved1;
    unsigned short Size; // ������ ����� ������, ������� ������� �� ����������
} tMESSAGEHEADER;
*/
//
typedef struct _NEXT_TRACK_COORD //
{
    int32_t Coord;
    int32_t LeftDebugCoord;
    int32_t RightDebugCoord;
    float  Speed;
    uint32_t Time;
} tNEXTTRACKCOORD;

typedef struct _JUMP_TRACK_COORD //
{
    int32_t Coord;
    int32_t LeftDebugCoord;
    int32_t RightDebugCoord;
    uint32_t Time;
}tJUMPTRACKCOORD;


typedef struct _OBJECT_DATA
{
    int32_t StartCoordinate;
    unsigned short Id;
}tOBJECTDATA;

#pragma pack(pop)

// �������������� ��������� ������ ���-��������
enum MessageId  //: unsigned char
{
ChangeCduModeId = 1,  //����� ������ CDU - �������
ChangeRcModeId = 2, //����� ������ RC � ����� (��������)
RegistrationOnId = 3, //�������� �����������
RegistrationOffId = 4, //��������� �����������
AnswerToQuestionId = 5, //����� �� ������
BoltJointOnId = 6, //������ �������� ���� ������
BoltJointOffId = 7, //������ �������� ���� ��������
OperatorTrackCoordinateId = 8, //���������� �������� ���������� (�� ��)
OperatorActionId = 9, //�������� ���������
DefectMarkId = 10, //������� � �������
RailroadSwitchMarkId = 11, //������� � �������
TrackMapId = 12, //������� ����
NextTrackCoordinateId = 13, //��������� ����������
JumpTrackCoordinateId = 14, //������ �� ���������� (������)
ManipulatorStateId = 15, //��������� ������������ (��������)
PingId = 16  //�������� ����������
};


#define PING_PERIOD 500 // ��

//
class UMUDEVICE: public QObject
{
    Q_OBJECT
    enum eState
    {
        Stopped = 0,
        CDUConnected,
        PCConnecting,
        WhenConnected,     // ��� ��������� ����������� �����������
        Working,
        Finishing      // ������������, �������� ������
    };

    enum eOutBufferIndex
    {
        CDUoutBufferIndex = 0,
        PCoutBufferIndex = 1,
        NumOfOutBuffers = 2 // ����� ����� �������
    };


public:
    static UMUDEVICE *deviceObjectPtr;

#ifdef DEFCORE_OS_WIN
     static class UNITWIN *_parentClass;
     static class cCriticalSection_Win* _critical_sectionPtr;
#else
     static class UNITLIN *_parentClass;
     static class cCriticalSection_Lin* _critical_sectionPtr;
#endif
    static std::queue<tLAN_CDUMessage> *_out_bufferPtr;  // ��������� �� ����� �������� ������ ��� ���
    static tLAN_CDUMessage _BScanMessage;
    static unsigned int _BScanMessageCounter;
    static bool *enablePLDIntPtr;   // ��������� �� ���� "����������" �� ���� ���������


    static PLDEMULATOR *pldLPtr;
    static PLDEMULATOR *pldRPtr;


    static char localIpAddress[];
    static char remoteIpAddress[];


    UMUDEVICE(cThreadClassList* ThreadClassList, void *parentClass /*cCriticalSection* CriticalSection*/);
    ~UMUDEVICE();

    bool _enablePLDInt;   // ���� "����������" �� ���� ���������

    eState getState(void);
    void start();

    void stop();

    bool engine();

    void writePLDRegister(eUMUSide side, unsigned int regAddress, unsigned char value);
    unsigned char readPLDRegister(eUMUSide side, unsigned int regAddress);

    unsigned int getNumberOfTacts();

    void printConnectionStatus();

signals:
    void CDUconnected();

public slots:
    void _onPLDInt(); // ������������ ������� _PLDIntTimer
    void _onPathStep(int shift, int coordInMM); // ���� �� ������ � ������������ �� �� trolley
    void _onPingTimer();

private:
    eState _state;
    bool _endWorkFlag;
    int _PCConnection_id;           // ������������� ���������� � cDatatr c �� ���������
    int _CDUConnection_id;          // ������������� ���������� � ���

    bool _CDUConnected;             // ���������� � ��� �����������
    bool _PCConnected;             // ���������� � �� �����������

    unsigned int _write_error_count[NumOfOutBuffers];

    std::queue<tLAN_CDUMessage> _CDU_out_buffer;
    std::queue<tLAN_PCMessage> _PC_out_buffer;

    unsigned char _receiveStartOffset;
    unsigned char _incmdbuf[UIP_BUFSIZE+4+cReceiveStartOffsetMax];

    cDataTransferLan* _dtLan;

    cCriticalSection* _critical_section[NumOfOutBuffers];
    cThreadClassList* _thlist;
    int _engineThreadIndex;

    QTimer _PLDIntTimer;

    PLDEMULATOR *_pldl;
    PLDEMULATOR *_pldr;

    TROLLEY *_pTrolley;
    EMULATOR *_pEmulator;
    QList<CID> _channelList;

    Test::eMovingDir _movingDirection; // ����������� ��������:

    QTimer _pingTimer;
    cCriticalSection* _pPingTimerCS;
    bool _needToPing;

    bool isEndWork();

    void unload(eOutBufferIndex outBufferIndex);
    bool Tick();
    void TickSend();
    void TickCDUReceive();
    void TickPCReceive();
    bool umuTick();

    void setState(eState newState);



protected:

    enum eReadState                       // ��������� �������� ������ ���������
    {
        rsOff = 0,   // ����������
        rsHead = 1,  // ������� ��������� ���������
        rsBody = 2,  // ������� ���� ���������
    };

    tLAN_PCMessage _currentMessage;
    eReadState _read_state;
    unsigned int _read_bytes_count;
    unsigned int _error_message_count;

    void readPCMessageHead(unsigned int& res, bool& fRepeat);
    void readPCMessageBody(unsigned int& res, bool& fRepeat);
    void unPack(tLAN_PCMessage &buf);
//
    void AddToOutBuffSync(tLAN_PCMessage* _out_block);
    void AddToOutBuffNoSync(tLAN_PCMessage* _out_block);
    void sendPingToPC();

    void whenTrolleyCoordChanged(int coordInMM);

//    void sendResponseWithData(const unsigned char id, const unsigned char* data, const unsigned short size);
};

#endif

