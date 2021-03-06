#ifndef _msg3204
#define _msg3204

#pragma pack(push,1)

// ���� � �������� (1) � ��������� (2) ������ ��� 3204

#ifdef OLD_PACKET_HEADER
#define hdrsize  4    // ������ ��������� 
typedef struct _LANMESSAGEHEADER
{
	unsigned char Id;
	unsigned char Sourse;
	unsigned short Size;
} tLANMESSAGEHEADER;
#else
#define hdrsize  6    //  new header size in bytes 
typedef struct _LANMESSAGEHEADER
{
	unsigned char Id;
	unsigned char Sourse;
	unsigned short Size;
	unsigned char MessageCount;
	unsigned char NotUse;
} tLANMESSAGEHEADER;
#endif

// ���������� ����� ���������, ����� ���� MessageCount
inline void fillMessageHeader(tLANMESSAGEHEADER *pHdr, unsigned char messageID, unsigned char sourseID,unsigned short size)
{
    pHdr->Id = messageID;
    pHdr->Sourse = sourseID;
    pHdr->Size = size;
#ifndef OLD_PACKET_HEADER
   pHdr->NotUse =  0;
#endif
}

// ������ ATTACH_MESSAGE_NUMBER � attachMessageNumber() � ���������� �� ������������
#ifndef OLD_PACKET_HEADER
#define ATTACH_MESSAGE_NUMBER_TO_HEADER(pHeader) attachMessageNumberToHeader(pHeader)
#else
#define ATTACH_MESSAGE_NUMBER_TO_HEADER(pHeader)
#endif

// 1.
// ������ ����� ������
#define idNtact 0x42
#define szNtact 1 // ����� �������������� �����

// ������ ��������� �����
#define idTactpar 0x40
typedef struct _TACTPARAMSCMD  // ������ ������ ���� ����� szDPim
{
    unsigned char TacktNumber;  
    unsigned char GenRcvLn1R;   // ����� 1, ������ �������
    unsigned char GenRcvLn2R;  
    unsigned char GenRcvLn1L;  // ����� 1, ����� �������
    unsigned char GenRcvLn2L;  
//
    unsigned char Duration;  
//
    unsigned char PulseFreqR;  
    unsigned char PulseFreqL;  
//
    unsigned char Gate0LevelLn1R;  
    unsigned char Gate1LevelLn1R;  
    unsigned char Gate2LevelLn1R;  
    unsigned char Gate3LevelLn1R;  
    unsigned char Gate0LevelLn2R;  
    unsigned char Gate1LevelLn2R;  
    unsigned char Gate2LevelLn2R;  
    unsigned char Gate3LevelLn2R;  
    unsigned char Gate0LevelLn1L;  
    unsigned char Gate1LevelLn1L;  
    unsigned char Gate2LevelLn1L;  
    unsigned char Gate3LevelLn1L;  
    unsigned char Gate0LevelLn2L;  
    unsigned char Gate1LevelLn2L;  
    unsigned char Gate2LevelLn2L;  
    unsigned char Gate3LevelLn2L;  
//
    unsigned char PulseAmpLn1R; 
    unsigned char PulseAmpLn2R; 
    unsigned char PulseAmpLn1L; 
    unsigned char PulseAmpLn2L; 
//

    unsigned char ACATTLn1R; // �������� ����������� � ���� �������� ��
    unsigned char ACATTLn2R;
    unsigned char ACATTLn1L;
    unsigned char ACATTLn2L;
//
    unsigned char SwitcherLn1R;    // 1(0) � ��������(���������) ���������� �������� B-���������
    unsigned char SwitcherLn2R;
    unsigned char SwitcherLn1L;
    unsigned char SwitcherLn2L;
//    
    unsigned char UnUsed[12];
} tTACTPARAMSCMD;

#define szTactpar 0x30
#define BscanSignalsCutOnMask 0x1 // ��� ���� SwitcherLnX(R/L)
/*
#if (szTactpar != sizeof(_TACTPARAMSCMD))
#error "The field -UnUsed- of tTACTPARAMSCMD must be corrected"
#endif
*/



// ������ ������ ���
#define idVRU     0x41
#define szVRU     0x05

// ������ ��������� ������
#define idStrobpar 0x49
#define szStrobpar 0x07

// ������ ����� � ������
#define id2Tp     0x4A
#define sz2Tp     0x03

// �������� �-���������
#define idOnAscan 0x45
#define szOnAscan 0x05

// ���������� ���
#define idBUMctrl 0x43
#define szBUMctrl 0x02


#define MAINSENSORSIGNMASK 0x80
#define IMITSIGNMASK                 0x08 //  � ���������� LAN-���������
#define SHIFTSENSORNUMBERMASK 3     // ������������� ����� �� ��� ��������� relativeNumber ��� �������� "��������"
// � ��������� ����� 3 ����, ����� - 2
#define TROLLEYSENSORNUMBER 0  // �����  ������� ��� ��� ��������� ��� deltaL �������
#define SCANERLENGTHSENSORNUMBER 1  // �����  ������� ��� ��� ��������� ��� deltaL �������
#define SCANERHEIGHTSENSORNUMBER 2  // �����  ������� ��� ��� ��������� ��� deltaH �������


// ���������� �������� ������� ���� ��� ���������
#define idDPset   0x1D
#define szDPset   0x05

// ������������� �� ��������� (������!) �������� ������� ���� ��� ��������� - ��� idDPset
#define szDPValAck 0x05

//-----------------------------------------------------------------------------
// ���������/�������� �������� ������� ����
#define idDPim    0x3E
typedef struct _DPImitCmd  // ������ ������ ���� ����� szDPim
{
    unsigned char ImitNumber;  
    unsigned char State;                    
} tDPIMITCMD;
#define szDPim  sizeof(tDPIMITCMD)  // 0x02
//-----------------------------------------------------------------------------
// ���������/�������� �������� ������� ����
#define idDPimEx    0x3F
typedef struct _DPImitCmdEx // ������ ������ ���� ����� szDPimEx
{
    tDPIMITCMD Main;
    unsigned short EventInterval;
} tDPIMITCMDEX;
#define szDPimEx sizeof(tDPIMITCMDEX) // 0x04
//-----------------------------------------------------------------------------
// ���������/�������� �������� �������
#define idDPScanImit    0x3C
typedef struct _DPScanImitCmd
{
    unsigned char State;                    
    unsigned short EventInterval;
    unsigned char AParameter;                    
    unsigned char BParameter;                    
} tDPSCANIMITCMD;
#define szDPScanImit   sizeof(tDPSCANIMITCMD)
//-----------------------------------------------------------------------------
// ������������ ������� ��������� �������� �� ������ "������-17"
#define idKSwitch  0x4F
#define szKSwitch   0x01
//---------------------------------
// 2.
// �-���������
#define idArazv   0x7F
#define szArazv   0xE9

// ������ �������� ��������� � �������� �-���������
#define idAAZ     0x82
#define szAAZ     0x04

// B- ���������
#define idBrazv    0x70
#define idLargeBrazv  0x72

// �- ���������
#define idMrazv   0x71

#define szBrazvmax 0x0D   // ������������ ����� �������������� ����� ����� B(M)-���������

// ���
#define idASD     0x83

// ���������� �� ������� ����
#define idDPval   0x3D

typedef struct _DPCOORDDATA
{
    unsigned char DPNumber;
    char Shift;
    int    PathCoord;
}tDPCOORDDATA;
#define szDPVal  sizeof(tDPCOORDDATA) // 0x06

//
typedef struct _DPCOORDMESSAGE
{
    tLANMESSAGEHEADER Header;
    tDPCOORDDATA Data;
} tDPCOORDMESSAGE;
#define szDPCoordMessage  sizeof(tDPCOORDMESSAGE)
//
//
// ���������� �� ��������� ������� ���� (����������� ������� �.4.31)
#define idDPvalEx   0x3E // ��������� �� ��M � ���
typedef struct _DPCOORDDATAEX
{
    unsigned char DPNumber;
    char Shift;
    int  PathCoord;
    unsigned int DisplayCoord;
}tDPCOORDDATAEX;
#define szDPValEx  sizeof(tDPCOORDDATAEX) // 0x08

typedef struct _DPCOORDMESSAGEEX
{
    tLANMESSAGEHEADER Header;
    tDPCOORDDATAEX Data;
} tDPCOORDMESSAGEEX;
#define szDPCoordMessageEx  sizeof(tDPCOORDMESSAGEEX)
//
#define MaxPositivePathCoord 0x7FFFFFFF
#define MaxNegativePathCoord 0x80000001
//
//����� ������ ���
#define idTime    0xC3
#define szTime    0x04

//������� � ���� ���������� � �������� ���������������
#define idDevID   0xC0
#define szDevID   0x01

//���������� ������������ ���
#define idVolt    0x11
#define szVolt    0x02

// ������ � ���� �������� �����
#define idSftw   0xDF
#define szSftw   0x09

// �������� ����� �����
#define idDevnum 0xDB
#define szDevnum 0x02

// ��������� ���������� ������� �������� ������������� ��������
#define idDefAScanSumTh 0x85
#define szDefAScanSumTh 0x02

// ������� �������� ������� �������� ������������� ��������
//define idSetAScanSumTh 0x87 // - ���������
#define szSetAScanSumTh 0x60


// ������������ �������� ��������� ����� � ����������� ��������
#define idJAndASensor 0x60
#define szJAndASensor 0x03
#define jointSensorId 0
#define cJointSensorJump 2 // �������: ������� ������� 0->1
#define cJointSensorFall    1 // �������: ������� ������� 1->0

#define idAScanSumAndTh 0x80
#define aScanSumBlock_sz 12 // ������ � ������ c ������ 2-� ������
#define aScanSumLineBlock_sz 6 // �������� �� aScanSumBlock_sz 

#define aScanThBlock_sz 12 // ������ � ������ c ������ 2-� ������
#define aScanThLineBlock_sz 6 // �������� �� aScanThBlock_sz

#define idAScanSumTh 0x88
#define szAScanSumTh  szSetAScanSumTh 




// ���������  ������� ������ ���������� �������� B-���������
#define idBScanSignalsCutStart 0x73
typedef struct _BSCANSIGNALSCUTSTARTCMD
{
    unsigned char SLT;
    unsigned char Start;
    unsigned char StartFrac;
} tBSCANSIGNALSCUTSTARTCMD;
#define szBScanSignalsCutStart sizeof(tBSCANSIGNALSCUTSTARTCMD) 

#define BscanSignalsCutStartMask 0xF  // ���� StartFrac

// ��������� ������������� ��������
#define idAcContactState 0x86
#define szAcContactStateBlock 2 // ������ � ������ c ������ 2-� ������
//#define szAcContactStateLineBlock 1 // �������� �� szAcContactStateBlock 
//
typedef struct _SPEEDMESSAGE
{
    tLANMESSAGEHEADER Header;
    unsigned short Speed; // �������� ������� � �������� ������� ���� � ������� (������������� �����)
} tSPEEDMESSAGE;

#define speedId 0x1C
#define speedSz 2


#pragma pack(pop)

#endif
