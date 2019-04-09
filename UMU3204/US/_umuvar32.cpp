// include-file
// исходный umuvar3.cpp
// 
// umuvar3.cpp
// ??
// umuvar2.cpp
// добавлено ifdef OLD_PACKET_HEADER

// umuvar1.cpp
// добавлено ifdef LARGE_BSCAN
//
#define idBUM 4
// fields offset() respectively start of message's body
#define slt_bidx 0

#ifdef OLD_PACKET_HEADER
// fields offset respectively start of message
#define slt_offs  4
#define asbdy_offs 5 // a-razv block
#else
#define slt_offs  6
#define asbdy_offs 7 // a-razv block

#endif

USHORT deviceSerialNumber;


#define tempbuf_size 512
__no_init UCHAR tempbuf[tempbuf_size];
//
#define o1data_size 16
__no_init UCHAR O1Data[o1data_size];
//
//
#define TprizmMax 250
#define TprizmLargeMax 1500


#define QDP   4  // ammount of DP  

//
UCHAR NumOfTacts;  // number of tacts - for 2 sides and lines


//
//USHORT WSA;
//USHORT vrchaddress,BA;

// format fAscanSend
// bit 0 - liniya 1, lev. storona
// bit 1 - liniya 2, lev. storona
// bit 2 - liniya 1, prav. storona
// bit 3 - liniya 2, prav. storona

DWORD AscanSendTick[2];  // [0] - liniya 1, [1] - liniya 2

#define AscanSendTime 50  // 6 mS

//
// format ascan[]...ascanstrobformax[]
// element 0 - liniya 1, lev. storona
// element 1 - liniya 2, lev. storona
// element  2 - liniya 1, prav. storona
// element  3 - liniya 2, prav. storona
// if ascan[] == 0 ascan is switch off
xSemaphoreHandle s_ascan,s_asd;
UCHAR ascan[4];
UCHAR curchan[4],ascanscale[4],ascanstart[4],ascanregim[4],ascanstrobformax[4];
//
//
//   ascan[]   2^x values
//      0   - otkl
//      1   - zapis parametrov v registry
//      2   - ozhidanie gotovnosti
//      4   - est gotovnost dannyh
//      8   - tekushii takt okonchen
//

UCHAR EnableASD;
DWORD ASDSendTick;
#define ASDSendTime 50  // mS

USHORT ASDtype[tactbitmsk+1];
//
// ASDSTR  ASD_PreBuf[tactbitmsk+1];

USHORT  ASD_PreBuf[tactbitmsk+1][5];
USHORT  ASD_Buffer[tactbitmsk+1];
//

// адреса п.п. вызываемых в цикле
__no_init vfuncv pShiftSensorProc[QDP]; // элемент соответствующий (mainShiftSensorNumber & SHIFTSENSORNUMBERMASK) равен NULL и не используется
// равно как и последний элемент


#define IMITATORMASK 0x4
UCHAR mainShiftSensorNumber; // абсолютный номер ДП или имитатора - признак "имитатор" - в разряде IMITATORMASK

unsigned char  ImitDPOn[QDP]; // если 0, имитатор ДП-x выключен, 1,2... - инкрементируется в ustsk() каждые  imitDPIncTime[]
// когда значение больше IMITDPWAIT, посылаем сообщение 3D


// для имитатора сканера ДП2 в режиме IMITDPSPECMODE1 + основной ДП
// за один цикл (инкремент) ДП2 производим:
//  1)  A - раз производим инкремент основного ДП
//  2)  2*A - раз производим декремент основного ДП
//  3)  A - раз производим инкремент основного ДП
unsigned char parameterA;     // параметр циклов основного ДП
unsigned char paramACounter;
unsigned char parameterB;   // количество циклов ДП2
unsigned char scanerImitStage; // см. SCANINITSTAGES

enum SCANIMITSTAGES
{
    SCANIMITINIT = 0,
    SCANIMITINC1 = 1,
    SCANIMITDEC = 2,
    SCANIMITINC2 = 3
};

unsigned char imitDPStep[QDP]; // см. IMITATIONDPSTEP

DWORD imitDPIncTime[QDP]; 
#define minImitDPIncTime 20   // минимальное значение imitDPIncTime[] в мС


xSemaphoreHandle s_ImitDP;  // 
DWORD imitDPIncTick[QDP];  


#ifndef DEVICE_EMULATION
const unsigned short *pDPValue[QDP] = 
{
    (unsigned short*)LENGTHDPVALUEADR,
    (unsigned short*)LENGTHDPVALUEADR,
    (unsigned short*)HEIGTDPVALUEADR,
    (unsigned short*)HEIGTDPVALUEADR // этот ДП не используется
};
#endif

int   Pathcoord[QDP*2];  // full coordinate of DP (signed!)
DWORD speedTimer;   // для расчета скорости - числа отсчетов основного ДП за время cSpeedCalcTime
// для отправки сообщения необходимо включить B-развертку
int mainShiftSensorPathcoordMemorized;
#define cSpeedCalcTime 500 // для расчета скорости - числа отсчетов основного ДП


unsigned int displayCoord; // дисплейная координата для основного ДП (для основного имитатора дисплейная координата не нужна)
// сбрасывается при выборе основного ДП или установке его значения
unsigned char fDPCoordDataExtended; // 0/1 - 1 - передавать данные о срабатывании основного ДП в расширенном формате (с дисплейной
//координатой см. п.4.31) Флаг устанавливается/сбрасывается с помощью команды управления работой БУМ (п.4.7 протокола)


//
#ifdef LARGE_BSCAN
xTaskHandle h_moveLargeBScan;
UCHAR  shiftValue; // со знаком
UCHAR fResetMScanTimer;

#else
USHORT StartBScanFIFO=0,EndBScanFIFO=0;
USHORT BScanFIFO[BufferSize];
UCHAR fMScan1;
#endif

//USHORT NumOfSignals;

UCHAR EnableBScan;
// fMScan - устанавливается каждый интервал MScanSendTime, если нет движения телеги. 
// Если (fMScan == 1) и нет сигналов, то передается только сообщение ДП с нулевым приращением координаты.
UCHAR fMScan;
DWORD MScanSendTick;
#define MScanSendTime 50  // mS - if it will be reached, set fMScan and
// send MScan

//
USHORT MaxAmp,MaxDelay1,MaxDelay2;

UCHAR flWasStopped=0;
UCHAR fIntPLDMustBeEnable; 

//
//
#define bscbufsize (hdrsize + szBrazvmax) // in bytes

DWORD bscbufL[(bscbufsize >> 2) + 1];      // для левой стороны
DWORD bscbufR[(bscbufsize >> 2) + 1];   // для правой стороны



//

//
// dly kazhdogo takta v DelayMultiplier
// element 0,low word  - liniya 1, lev. storona
// element 1,low word  - liniya 2, lev. storona
// element 0,high word - liniya 1, prav. storona
// element 1,high word - liniya 2, prav. storona
// element:
// bit <2..0> -  nomer stroba 1...4 - ogib. No.1
// bit <6..4> -  nomer stroba 1...4 - ogib. No.2
// bit <10..8> - nomer stroba 1...4 - ogib. No.3
DWORD enveloptakt[tactbitmsk+1][2];
//
//
//
DWORD ascancounter = 0;

#ifndef OLD_PACKET_HEADER
UCHAR messageNumber;
#endif

__no_init DWORD versionInfoSendTick;  
__no_init DWORD versionInfoTimeout;
__no_init BOOL jointSensorDisable;


USHORT jointSensorState;

BOOL fIsTrolleyDP;        // признак FALSE/TRUE - подключен датчик разъема ДП/ датчик dL сканера

#ifndef AC_dis
//
// Разрешение передачи сообщений о состоянии акустического контакта (сырые данные - сумма значений A-развертки)
// разрешиние может установлено, если уже разрешена B-развертка
// при отключении B-развертки, если еще снято, снимается 
UCHAR enableACMessage;  // см. ENABLEACMESSAGE


// значения порогов, записанные в область параметров тактов также запоминаем сюда, чтобы не читать ее при
// работающей ПЛИС, однако здесь аргумент - номер приемника, а не такта
// информация требуется для 0x80, 0x88
tACThLine *pAcThLine0;
tACThLine *pAcThLine1;
unsigned char receiversInCycle[2][2][tactbitmsk+1]; // номера используемых приемников в цикле зондирования - сторона-линия-такт

ACTHRARRAY *pACMaxSums;
unsigned int ACThCalcPeriodByCycle;  //  число циклов, через которое запускается пересчет порогов АК
#define cACThCalcPeriodByCycle 30000 //
unsigned int acThCalcPeriodCnt;  // счетчик этих циклов
#define cACSumGatheringPeriodByCycle 10 //  число циклов, через которое запускается цикл подсчетов сумм для накопления

unsigned char acThPercent;
typedef enum _ACCALCSTAGE
{
    ACCALC_NEED_TO_START = 0,
    ACCALC_WAITFORDATA = 1,   // ожидание готовности сумм для оценки АК
//
    ACTHCALC_CNTQUERY = 2, // состояние, когда обнулился acThCalcPeriodCnt
    ACTHCALC_WAITFORDATA = 3, // ожидание готовности сумм для подсчета порогов - настройка
    ACTHCALC_COMPUTEQUERY = 4,
    ACTHCALC_COMPUTEEXEC = 5 // вычисление порогов
} eACCALCSTAGE;

eACCALCSTAGE ACCalcStage;
unsigned char gatheringSumCnt; // если не ноль, осуществляем накопление сумм АК без рассчета порогов и выдачи
// данных пользователю. Инициализируется при включении АК значением (ACTHRARRAY_DEPTH-min), где min - минимальное
// из значений полей used структур tARRAYOFSUMS, используемых при данной таблице тактов
//
#define cGATHERINGSUMDURATION 50 // начальное значение gatheringSumCnt
xSemaphoreHandle s_ACTH;

// интервал в отсчетах ДП, по достижению которого производится отсылка данных 
// о состоянии АК, если состояние не изменялось. Если состояние изменилось, то отсылка данных производится сразу же
// без учета пройденного пути
#define cACStateSendInterval_Pos 20
#define cACStateSendInterval_Neg 236 // т.е. -20


unsigned char ACStateSendIntervalCntr; // однако число со знаком
bool fACStateSendIntervalReached; 

#endif
