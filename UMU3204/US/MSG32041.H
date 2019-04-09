#ifndef _msg3204
#define _msg3204

#pragma pack(push,1)

// коды и входящих (1) и исходящих (2) команд БУМ 3204

#ifdef OLD_PACKET_HEADER
#define hdrsize  4    // старый заголовок 
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

// заполнение полей заголовка, кроме поля MessageCount
inline void fillMessageHeader(tLANMESSAGEHEADER *pHdr, unsigned char messageID, unsigned char sourseID,unsigned short size)
{
    pHdr->Id = messageID;
    pHdr->Sourse = sourseID;
    pHdr->Size = size;
#ifndef OLD_PACKET_HEADER
   pHdr->NotUse =  0;
#endif
}

// макрос ATTACH_MESSAGE_NUMBER и attachMessageNumber() в дальнейшем не использовать
#ifndef OLD_PACKET_HEADER
#define ATTACH_MESSAGE_NUMBER_TO_HEADER(pHeader) attachMessageNumberToHeader(pHeader)
#else
#define ATTACH_MESSAGE_NUMBER_TO_HEADER(pHeader)
#endif

// 1.
// задать число тактов
#define idNtact 0x42
#define szNtact 1 // длина информационной части

// задать параметры такта
#define idTactpar 0x40
typedef struct _TACTPARAMSCMD  // размер должен быть равен szDPim
{
    unsigned char TacktNumber;  
    unsigned char GenRcvLn1R;   // линия 1, правая сторона
    unsigned char GenRcvLn2R;  
    unsigned char GenRcvLn1L;  // линия 1, левая сторона
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

    unsigned char ACATTLn1R; // величина аттенюатора в зоне контроля АК
    unsigned char ACATTLn2R;
    unsigned char ACATTLn1L;
    unsigned char ACATTLn2L;
//
    unsigned char SwitcherLn1R;    // 1(0) – включить(отключить) фильтрацию сигналов B-развертки
    unsigned char SwitcherLn2R;
    unsigned char SwitcherLn1L;
    unsigned char SwitcherLn2L;
//    
    unsigned char UnUsed[12];
} tTACTPARAMSCMD;

#define szTactpar 0x30
#define BscanSignalsCutOnMask 0x1 // для поля SwitcherLnX(R/L)
/*
#if (szTactpar != sizeof(_TACTPARAMSCMD))
#error "The field -UnUsed- of tTACTPARAMSCMD must be corrected"
#endif
*/



// задать кривую ВРЧ
#define idVRU     0x41
#define szVRU     0x05

// задать параметры строба
#define idStrobpar 0x49
#define szStrobpar 0x07

// задать время в призме
#define id2Tp     0x4A
#define sz2Tp     0x03

// включить А-развертку
#define idOnAscan 0x45
#define szOnAscan 0x05

// управление БУМ
#define idBUMctrl 0x43
#define szBUMctrl 0x02


#define MAINSENSORSIGNMASK 0x80
#define IMITSIGNMASK                 0x08 //  в сообщениях LAN-протокола
#define SHIFTSENSORNUMBERMASK 3     // относительный номер ДП или имитатора relativeNumber без признака "имитатор"
// в протоколе номер 3 бита, здесь - 2
#define TROLLEYSENSORNUMBER 0  // номер  датчика или его имитатора для deltaL тележки
#define SCANERLENGTHSENSORNUMBER 1  // номер  датчика или его имитатора для deltaL сканера
#define SCANERHEIGHTSENSORNUMBER 2  // номер  датчика или его имитатора для deltaH сканера


// установить значение датчика пути или имитатора
#define idDPset   0x1D
#define szDPset   0x05

// подтверждение на установку (только!) значения датчика пути или имитатора - код idDPset
#define szDPValAck 0x05

//-----------------------------------------------------------------------------
// отключить/включить имитатор датчика пути
#define idDPim    0x3E
typedef struct _DPImitCmd  // размер должен быть равен szDPim
{
    unsigned char ImitNumber;  
    unsigned char State;                    
} tDPIMITCMD;
#define szDPim  sizeof(tDPIMITCMD)  // 0x02
//-----------------------------------------------------------------------------
// отключить/включить имитатор датчика пути
#define idDPimEx    0x3F
typedef struct _DPImitCmdEx // размер должен быть равен szDPimEx
{
    tDPIMITCMD Main;
    unsigned short EventInterval;
} tDPIMITCMDEX;
#define szDPimEx sizeof(tDPIMITCMDEX) // 0x04
//-----------------------------------------------------------------------------
// отключить/включить имитатор сканера
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
// переключение каналов сплошного контроля на сканер "Авикон-17"
#define idKSwitch  0x4F
#define szKSwitch   0x01
//---------------------------------
// 2.
// А-развертка
#define idArazv   0x7F
#define szArazv   0xE9

// точное значение амплитуды и задержки А-развертки
#define idAAZ     0x82
#define szAAZ     0x04

// B- развертка
#define idBrazv    0x70
#define idLargeBrazv  0x72

// М- развертка
#define idMrazv   0x71

#define szBrazvmax 0x0D   // максимальная длина информационной части кадра B(M)-развертки

// АСД
#define idASD     0x83

// координата от датчика пути
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
// координата от основного датчика пути (расширенный вариант п.4.31)
#define idDPvalEx   0x3E // сообщение от БУM к БУИ
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
//время работы БУМ
#define idTime    0xC3
#define szTime    0x04

//наличие в сети устройства с заданным идентификатором
#define idDevID   0xC0
#define szDevID   0x01

//напряжение аккумулятора БУМ
#define idVolt    0x11
#define szVolt    0x02

// версия и дата прошивки блока
#define idSftw   0xDF
#define szSftw   0x09

// серийный номер блока
#define idDevnum 0xDB
#define szDevnum 0x02

// Параметры вычисления порогов проверки акустического контакта
#define idDefAScanSumTh 0x85
#define szDefAScanSumTh 0x02

// Задание значений порогов проверки акустического контакта
//define idSetAScanSumTh 0x87 // - исключена
#define szSetAScanSumTh 0x60


// срабатывание датчиков болтового стыка и стрелочного перевода
#define idJAndASensor 0x60
#define szJAndASensor 0x03
#define jointSensorId 0
#define cJointSensorJump 2 // событие: переход датчика 0->1
#define cJointSensorFall    1 // событие: переход датчика 1->0

#define idAScanSumAndTh 0x80
#define aScanSumBlock_sz 12 // размер в байтах c учетом 2-х сторон
#define aScanSumLineBlock_sz 6 // половина от aScanSumBlock_sz 

#define aScanThBlock_sz 12 // размер в байтах c учетом 2-х сторон
#define aScanThLineBlock_sz 6 // половина от aScanThBlock_sz

#define idAScanSumTh 0x88
#define szAScanSumTh  szSetAScanSumTh 




// Установка  момента начала фильтрации сигналов B-развертки
#define idBScanSignalsCutStart 0x73
typedef struct _BSCANSIGNALSCUTSTARTCMD
{
    unsigned char SLT;
    unsigned char Start;
    unsigned char StartFrac;
} tBSCANSIGNALSCUTSTARTCMD;
#define szBScanSignalsCutStart sizeof(tBSCANSIGNALSCUTSTARTCMD) 

#define BscanSignalsCutStartMask 0xF  // поле StartFrac

// Состояние акустического контакта
#define idAcContactState 0x86
#define szAcContactStateBlock 2 // размер в байтах c учетом 2-х сторон
//#define szAcContactStateLineBlock 1 // половина от szAcContactStateBlock 
//
typedef struct _SPEEDMESSAGE
{
    tLANMESSAGEHEADER Header;
    unsigned short Speed; // скорость прибора в отсчетах датчика пути в секунду (положительное число)
} tSPEEDMESSAGE;

#define speedId 0x1C
#define speedSz 2


#pragma pack(pop)

#endif
