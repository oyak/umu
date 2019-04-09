// include-file
// �������� umuvar3.cpp
// 
// umuvar3.cpp
// ??
// umuvar2.cpp
// ��������� ifdef OLD_PACKET_HEADER

// umuvar1.cpp
// ��������� ifdef LARGE_BSCAN
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

// ������ �.�. ���������� � �����
__no_init vfuncv pShiftSensorProc[QDP]; // ������� ��������������� (mainShiftSensorNumber & SHIFTSENSORNUMBERMASK) ����� NULL � �� ������������
// ����� ��� � ��������� �������


#define IMITATORMASK 0x4
UCHAR mainShiftSensorNumber; // ���������� ����� �� ��� ��������� - ������� "��������" - � ������� IMITATORMASK

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
#define minImitDPIncTime 20   // ����������� �������� imitDPIncTime[] � ��


xSemaphoreHandle s_ImitDP;  // 
DWORD imitDPIncTick[QDP];  


#ifndef DEVICE_EMULATION
const unsigned short *pDPValue[QDP] = 
{
    (unsigned short*)LENGTHDPVALUEADR,
    (unsigned short*)LENGTHDPVALUEADR,
    (unsigned short*)HEIGTDPVALUEADR,
    (unsigned short*)HEIGTDPVALUEADR // ���� �� �� ������������
};
#endif

int   Pathcoord[QDP*2];  // full coordinate of DP (signed!)
DWORD speedTimer;   // ��� ������� �������� - ����� �������� ��������� �� �� ����� cSpeedCalcTime
// ��� �������� ��������� ���������� �������� B-���������
int mainShiftSensorPathcoordMemorized;
#define cSpeedCalcTime 500 // ��� ������� �������� - ����� �������� ��������� ��


unsigned int displayCoord; // ���������� ���������� ��� ��������� �� (��� ��������� ��������� ���������� ���������� �� �����)
// ������������ ��� ������ ��������� �� ��� ��������� ��� ��������
unsigned char fDPCoordDataExtended; // 0/1 - 1 - ���������� ������ � ������������ ��������� �� � ����������� ������� (� ����������
//����������� ��. �.4.31) ���� ���������������/������������ � ������� ������� ���������� ������� ��� (�.4.7 ���������)


//
#ifdef LARGE_BSCAN
xTaskHandle h_moveLargeBScan;
UCHAR  shiftValue; // �� ������
UCHAR fResetMScanTimer;

#else
USHORT StartBScanFIFO=0,EndBScanFIFO=0;
USHORT BScanFIFO[BufferSize];
UCHAR fMScan1;
#endif

//USHORT NumOfSignals;

UCHAR EnableBScan;
// fMScan - ��������������� ������ �������� MScanSendTime, ���� ��� �������� ������. 
// ���� (fMScan == 1) � ��� ��������, �� ���������� ������ ��������� �� � ������� ����������� ����������.
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

DWORD bscbufL[(bscbufsize >> 2) + 1];      // ��� ����� �������
DWORD bscbufR[(bscbufsize >> 2) + 1];   // ��� ������ �������



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

BOOL fIsTrolleyDP;        // ������� FALSE/TRUE - ��������� ������ ������� ��/ ������ dL �������

#ifndef AC_dis
//
// ���������� �������� ��������� � ��������� ������������� �������� (����� ������ - ����� �������� A-���������)
// ���������� ����� �����������, ���� ��� ��������� B-���������
// ��� ���������� B-���������, ���� ��� �����, ��������� 
UCHAR enableACMessage;  // ��. ENABLEACMESSAGE


// �������� �������, ���������� � ������� ���������� ������ ����� ���������� ����, ����� �� ������ �� ���
// ���������� ����, ������ ����� �������� - ����� ���������, � �� �����
// ���������� ��������� ��� 0x80, 0x88
tACThLine *pAcThLine0;
tACThLine *pAcThLine1;
unsigned char receiversInCycle[2][2][tactbitmsk+1]; // ������ ������������ ���������� � ����� ������������ - �������-�����-����

ACTHRARRAY *pACMaxSums;
unsigned int ACThCalcPeriodByCycle;  //  ����� ������, ����� ������� ����������� �������� ������� ��
#define cACThCalcPeriodByCycle 30000 //
unsigned int acThCalcPeriodCnt;  // ������� ���� ������
#define cACSumGatheringPeriodByCycle 10 //  ����� ������, ����� ������� ����������� ���� ��������� ���� ��� ����������

unsigned char acThPercent;
typedef enum _ACCALCSTAGE
{
    ACCALC_NEED_TO_START = 0,
    ACCALC_WAITFORDATA = 1,   // �������� ���������� ���� ��� ������ ��
//
    ACTHCALC_CNTQUERY = 2, // ���������, ����� ��������� acThCalcPeriodCnt
    ACTHCALC_WAITFORDATA = 3, // �������� ���������� ���� ��� �������� ������� - ���������
    ACTHCALC_COMPUTEQUERY = 4,
    ACTHCALC_COMPUTEEXEC = 5 // ���������� �������
} eACCALCSTAGE;

eACCALCSTAGE ACCalcStage;
unsigned char gatheringSumCnt; // ���� �� ����, ������������ ���������� ���� �� ��� �������� ������� � ������
// ������ ������������. ���������������� ��� ��������� �� ��������� (ACTHRARRAY_DEPTH-min), ��� min - �����������
// �� �������� ����� used �������� tARRAYOFSUMS, ������������ ��� ������ ������� ������
//
#define cGATHERINGSUMDURATION 50 // ��������� �������� gatheringSumCnt
xSemaphoreHandle s_ACTH;

// �������� � �������� ��, �� ���������� �������� ������������ ������� ������ 
// � ��������� ��, ���� ��������� �� ����������. ���� ��������� ����������, �� ������� ������ ������������ ����� ��
// ��� ����� ����������� ����
#define cACStateSendInterval_Pos 20
#define cACStateSendInterval_Neg 236 // �.�. -20


unsigned char ACStateSendIntervalCntr; // ������ ����� �� ������
bool fACStateSendIntervalReached; 

#endif
