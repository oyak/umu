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
#include "config.h"
#include "logfile.h"


//define SKIP_CDU_CONNECTING
//define SKIP_PC_CONNECTING

//define PATH_MAP_LOGFILE_ON // включить запись лога по карте пути
//define TESTING_PATH_MAP_ON // включить проверку допустимых зазоров между объектами пути

//define PCLAN_MESSAGE_LOGFILE_ON // включить запись лога LAN-сообщений от тренажера

#ifdef SKIP_CDU_CONNECTING
#ifdef SKIP_PC_CONNECTING
#error "Ошибка: в проекте все ethernet-соединения заблокированы"
#endif
#endif


#define N0EMS_SENSOR_SHIFT_mm 90  // смещение ПЭП 0гр в искательной системе относительно ее
// центра (между БР1 и БР2), мм

#define cReceiveStartOffsetMax 0x35  // самое длинное сообщение протокола для БУМ минус 1

#define LAN_MESSAGE_SHORT_HEADER_SIZE 4
#define LAN_MESSAGE_BIG_HEADER_SIZE 6

// идентификаторы сообщений обмена БУМ-тренажер
enum MessageId  //: unsigned char
{
    ChangeCduModeId = 1,            //смена режима CDU - планшет
    ChangeRcModeId = 2,             //смена режима RC – пульт (смартфон)
    RegistrationOnId = 3,           //включена регистрация
    RegistrationOffId = 4,          //выключена регистрация
    AnswerToQuestionId = 5,         //ответ на вопрос
    BoltJointOnId = 6,              //кнопка болтовой стык нажата
    BoltJointOffId = 7,             //кнопка болтовой стык отпущена
    OperatorTrackCoordinateId = 8,  //координата отмечена оператором (км пк)
    OperatorActionId = 9,           //действие оператора
    DefectMarkId = 10,              //отметка о дефекте
    RailroadSwitchMarkId = 11,      //отметка о стрелке
    TrackMapId = 12,                //маршрут пути
    NextTrackCoordinateId = 13,     //следующая координата
    JumpTrackCoordinateId = 14,     //прыжок на координату (разбор)
    ManipulatorStateId = 15,        //состояние манипулятора (скорость)
    PingId = 16                     //контроль соединения
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




#define PING_PERIOD 500             // мс
#define PC_LINK_FAULT_TIMEOUT 3000  // мс

//
class UMUDEVICE : public QObject
{
    Q_OBJECT
    enum eState
    {
        Stopped = 0,
        CDUConnected,
        PCConnecting,
        WhenConnected,  // все требуемые подключения установлены
        Working,
        Finishing  // разъединение, закрытие тредов
    };

    enum eOutBufferIndex
    {
        CDUoutBufferIndex = 0,
        PCoutBufferIndex = 1,
        NumOfOutBuffers = 2  // общее число буферов
    };


public:
    static UMUDEVICE* deviceObjectPtr;

#ifdef DEFCORE_OS_WIN
    static class UNITWIN* _parentClass;
    static class cCriticalSection_Win* _critical_sectionPtr;
#else
    static class UNITLIN* _parentClass;
    static class cCriticalSection_Lin* _critical_sectionPtr;
#endif
    static std::queue<tLAN_CDUMessage>* _out_bufferPtr;  // указатель на буфер выгрузки данных для БУИ
    static tLAN_CDUMessage _BScanMessage;
    static unsigned int _BScanMessageCounter;
    static bool* enablePLDIntPtr;  // указатель на флаг "прерывания" от ПЛИС разрешены


    static PLDEMULATOR* pldLPtr;
    static PLDEMULATOR* pldRPtr;


    static char localIpAddress[];
    static char remoteIpAddress[];

    UMUDEVICE(cThreadClassList* ThreadClassList, void* parentClass, CONFIG* pConfig);
    ~UMUDEVICE();

    bool _enablePLDInt;  // флаг "прерывания" от ПЛИС разрешены

    eState getState(void);
    void start();

    void stop();

    bool engine();

    void writePLDRegister(eUMUSide side, unsigned int regAddress, unsigned char value);
    unsigned char readPLDRegister(eUMUSide side, unsigned int regAddress);

    void writeIntoRAM(eUMUSide side, unsigned int regAddress, unsigned char value);
    unsigned char readFromRAM(eUMUSide side, unsigned int regAddress);

    unsigned int getNumberOfTacts();

    void printConnectionStatus();

    // работа с настройками
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
#ifdef _test_message_numeration_integrity
    void testMessageNumerationIntegrity(tLAN_CDUMessage* _out_block);
#endif
    //
signals:
    void CDUconnected();
    void restartPCLinkFaultTimer();
    void message(QString s);

public slots:
    void _onPLDInt();                                             // срабатывание таймера _PLDIntTimer
    void _onPathStep(int shift, int coordLInMM, int coordRInMM);  // слот на сигнал о срабатывании ДП от trolley
    void _onPingTimer();
    void _onPCLinkFaultTimer();         // слот на срабатывание _PCLinkFaultTimer
    void _onRestartPCLinkFaultTimer();  // перезапускаем _PCLinkFaultTimer, чтобы не сработал
    void onMessage(QString s); // слот на сигналы с текстовыми сообщениями от используемых классов
    void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg); // обработчик сообщений из QDebug

private:
    eState _state;
    bool _endWorkFlag;
    int _PCConnection_id;   // Идентификатор соединения в cDatatr c ПК тренажера
    int _CDUConnection_id;  // идентияикатор соединения с БУИ

    bool _CDUConnected;  // соединение с БУИ установлено
    bool _PCConnected;   // соединение с ПК установлено

    bool _PCLinkFault;  // устанавливаем, если нет PingId от ПК в течение PC_LINK_FAULT_TIMEOUT
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

    QTimer _PLDIntTimer;

    PLDEMULATOR* _pldl;
    PLDEMULATOR* _pldr;

    TROLLEY* _pTrolley;
    EMULATOR* _pEmulator;
    QList<CID> _channelList;

    Test::eMovingDir _movingDirection;  // направление движения:

    QTimer _pingTimer;
    cCriticalSection* _pPingTimerCS;
    bool _needToPing;
    CONFIG* _pConfig;

    LOGFILE *_pPathMapLogFile;
    LOGFILE *_pLANPCMessageLogFile;

    bool isEndWork();

    void unload(eOutBufferIndex outBufferIndex);
    bool CDUTick();
    bool PCTick();
    void CDUTickSend();
    void PCTickSend();
    void TickCDUReceive();
    void TickPCReceive();
    bool umuTick();

    void setState(eState newState);


protected:
    enum eReadState  // Состояние процесса чтения сообщения
    {
        rsOff = 0,   // Выключенно
        rsHead = 1,  // Ожидаем заголовок сообщения
        rsBody = 2,  // Ожидаем тело сообщения
        rsTestHead = 3, // проверка заголовка
        rsSkipWrongId = 4, // выбырасываем недействительный идентификатор
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

#endif
