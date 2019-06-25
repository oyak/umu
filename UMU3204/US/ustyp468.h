// исходный ustyp466.h

#ifndef _ustyp468h
#define _ustyp468h


#include "syshrdw.inc"
#include "mvnewa.h"
#include "MSG32041.H"
//
#ifndef ramstart
#define ramstart 0
#endif
//
#define sidebitmsk 0x80
#define linebitmsk 0x40
#define tactbitmsk 0x1F

#define NumberOfReceivers 8 // число приемников на каждой линии
#define RECEIVER_UNKNOWN 0xFF
//
#define ACTHRARRAY_DEPTH 10
typedef struct _ARRAYOFSUMS
{
    DWORD Sums[ACTHRARRAY_DEPTH+1];
    unsigned int used; // число заполненных элементов массива из 1..ACTHRARRAY_DEPTH

} tARRAYOFSUMS;
// в массиве Sums  элемент с номером (ACTHARRAY_DEPTH) соответствует самому старому
// измерению, с номерами 0 и 1 - последнему. В элемент 0 записываются данные при обработке прерывания. В цикле
// ACTHRARRAY_DEPTH-й элемент выкидывается, на его место записывается (ACTHRARRAY_DEPTH-1)-й ... на место первого -
// нулевой. Из элементов ACTHRARRAY_DEPTH...1 находим максимальный, его используем для вычисления порога

typedef tARRAYOFSUMS ACTHRARRAY[2][2][NumberOfReceivers]; // массивы сумм А-развертки в циклах зондирования
// в формате строна-линия-приемник.

#define cMaxAscanSumTh 0xFFFFFF // порог задается величиной не длиннее 3 байт - порог не определен, т.к.
// предполагается, что мы его не можем достигнуть

#define BScanASDBaseAddr 0x11
#define AScanStartAddr1 0x20
#define AScanStartAddr2 0x21

#define AscanSumAdr1 0x2200
#define AscanSumAdr2 0x2300
#define ACStateAdr1     0x2203
#define ACStateAdr2     0x2303


//
#define BScanASD_0 (ramstart+(BScanASDBaseAddr<<9))
#define BScanASD_1 (ramstart+(BScanASDBaseAddr<<9)+2)

//
#define BScanASDCRegAddr (ramstart+(0x10<<9))
// BScanASDCRegAddr  на запись
// бит 7 = 1  - запуск B-развертки
// биты 6...3 - число тактов
// в BScanASDCRegAddr  перед чтением записываем
// бит 0 = устанавливаем 0/1 перед  чтением  линии 1/2
// бит 1 - АСД/B-разветка   - 0/1

//
//  количество блоков рано числу тактов
//  данные в блоке - байты 0..2 - сумма значений A-развертки; бит 0 в байте 3  установлен если есть превышение суммы порогом
#define ACLineBlockStart_1 (ramstart+(AscanSumAdr1<<1)) // линия 1
#define ACLineBlockStart_2 (ramstart+(AscanSumAdr2<<1))

#define ACStateStartLine_1 (ramstart+(ACStateAdr1<<1)) // линия 1
#define ACStateStartLine_2 (ramstart+(ACStateAdr2<<1))

#define szPLD_ACLineBlock 4

#define cTactMaskStep 0x80
#define bScanSignalBlock_sz 14  // размер блока данных о сигнале B-развертки (см.протокол) в байтах



#define a0x1300 (ramstart+(0x1300<<1))
#define a0x1301 (ramstart+(0x1301<<1)) // число тактов

//#define a0x1303 (ramstart+(0x1303<<1))
//#define a0x1381 (ramstart+(0x1381<<1))

#define a0x1307 (ramstart+(0x1307<<1))
//

#define a0x13A0 (ramstart+(0x13A0<<1)) // если биты 9,1 установлены, то время в призме 0...150 мкС,
                                                                    // если сброшен - 0..25 мкс
//
#define ACSumStartReg (ramstart+(0x1311<<1)) // установка бита 0 -> запуск цикла подсчета АК, если АК включен
#define ACSumStartValue ((1 << 8) | 1)
//
#define ACControlReg (ramstart+(0x1310<<1))
// бит 0 - вкл/откл (1/0)
// бит 1 - тип АК цикла настройка/оценка (1/0)
#define ACControlValue_OFF 0
#define ACControlValue_TUNE ((3 << 8) | 3)
#define ACControlValue_EVAL ((1 << 8) | 1)

//
#define ExtRamStartAdr (ramstart+ extramstart)
//
#define  Aaddr1 (ramstart+(AScanStartAddr1<<9))
#define  Aaddr2 (ramstart+(AScanStartAddr2<<9))
//
//
#define BufferSize 4096 // increased *2 and USHORT instead of UCHAR
//
#define Lo(v) *(UCHAR*)&v
#define Hi(v) *((UCHAR*)&v+1)
//
/*
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef ulong
#define ulong unsigned long
#endif
*/

typedef USHORT ASDSTR[5];
//
#define minASDPeriod 500       //  мкС
#define t1MinimumMatch (  minASDPeriod * SYSFREQ /4)

//
// мимимальное значение для  ЦАП, передаваемое в сообщении
#define c_dacminval 20 // без учета c_dacCorrection

// максимальное значение, для ЦАП, передаваемое в сообщении
#define c_dacmaxval 180 // без учетоа c_dacCorrection
//
#define c_dacCorrection  12 // c учетом того, что напряжение нужно увеличить на 0,15 В
//

#define parreg_sz 0x200 // length of tact parapeter region in bytes - 0x100 words

// смещения параметров такта
#define _ChLCRmask    (1<<1)
#define _TprizmL1mask (3<<1)
#define _TprizmR1mask (4<<1)
#define _WSALmask     (5<<1)
#define _RazvCRmask   (7<<1)
#define _LevStrL0mask (8<<1)
#define _LevStrR0mask (12<<1)
#define _AMPZI1mask   (16<<1)
#define _TprizmL2mask (18<<1)
#define _TprizmR2mask (19<<1)

#define _TaktFindMaxLmax  (20<<1)   // номер такта 0 - канал
#define _Takt2FindMaxLmax (24<<1)   // номер такта 1 - канал
#define _Takt3FindMaxLmax (26<<1)   // номер такта 2 - канал

#define _StrobFindMaxLmask (22<<1)  // здесь задаются номера стробов для всех каналов
#define _StrobFindMaxRmask (23<<1)
//
#define _FreqL1mask (28<<1)  // бит 0 - частота ЗИ, линия 1:  0/1 - 2.5/5.0 МГц
#define _FreqL2mask (29<<1)

// параметры "момент начала подавления сигналов B-развертки" для линий 1,2
#define BScanSignalsCutOnParamBitNum 7
#define BScanSignalsCutStartFracParamBitMask 0xF

#define BScanCutStart1mask (32<<1)  // целые доли мкс.
#define BScanCutStartFrac1mask (33<<1) // бит BScanSignalsCutOnParamBitNum - включить, биты 6..4 - не используются, биты 3..0 - дес.доли мкс
#define BScanCutStart2mask (34<<1) // целые доли мкс.
#define BScanCutStartFrac2mask (35<<1)  // бит BScanSignalsCutOnParamBitNum - включить, биты 6..4 - не используются, биты 3..0 - дес.доли мкс

#define _AScanSumTh1mask (36<<1)  // порог суммы А-развертки, линия 1, 3 байта
#define _AScanSumTh2mask (39<<1)  // порог суммы А-развертки, линия 2, 3 байта



//
#define cVersionInfoTimeoutMin 5000
//
#define Gc(a,b) (a | (b<<16))
//
// dstaddr - byte's pointer

#ifdef OLD_PACKET_HEADER
#define fill0x82_hdr(dstaddr)    \
{ \
   *(dstaddr)  = idAAZ;          \
   *(dstaddr+1) = idBUM;         \
   *(dstaddr+2) = szAAZ & 0xFF;  \
   *(dstaddr+3) = szAAZ >> 8;    \
}
#else
#define fill0x82_hdr(dstaddr)    \
{ \
   *(dstaddr)  = idAAZ;          \
   *(dstaddr+1) = idBUM;         \
   *(dstaddr+2) = szAAZ & 0xFF;  \
   *(dstaddr+3) = szAAZ >> 8;    \
   *(dstaddr+5) = 0;    \
}
#endif
//-------------------------------------------------------------------
#ifdef OLD_PACKET_HEADER
#define fill0x7F_hdr(dstaddr)        \
{ \
   *(dstaddr)  = idArazv;            \
   *(dstaddr+1) = idBUM;           \
   *(dstaddr+2) = szArazv & 0xFF;  \
   *(dstaddr+3) = szArazv >> 8;    \
}
#else
#define fill0x7F_hdr(dstaddr)        \
{ \
   *(dstaddr)  = idArazv;            \
   *(dstaddr+1) = idBUM;           \
   *(dstaddr+2) = szArazv & 0xFF;  \
   *(dstaddr+3) = szArazv >> 8;    \
   *(dstaddr+5) = 0;    \
}
#endif
//-------------------------------------------------------------------


#define SideLeft 0x80   // Левая сторона
#define indexLeft 1   // Левая сторона

#define SideRight  0x0  // Правая сторона
#define indexRight 0  // Правая сторона

// управление состоянием реле К
// переключения каналов сплошного контроля на сканер "Авикон-17"
enum
{
   KSignalsOff = 0,
   resetKCmd = 1,
   setKCmd = 2
};
#define cRelayCtrlSignalDur 20  // время удержания сигнала на входах реле, мС


enum IMITDPONSTATE
{
  IMITDPOFF = 0,
  IMITDPWAIT = 1,
  IMITDPSENDF  = 2,         // переменная может пробегать значения от IMITDPSENDF до IMITDPSENDL
  IMITDPSENDL = 0x7F,
  IMITDPSPECMODE1 = 0xFF  // имитатор сканера (только ДП-2 как дополнительный)
};

enum IMITATIONDPSTEP
{
    DPSTEPFORWARD = 1,
    DPSTEPBACKWARD = 0xFF    //  unsigned char (-1)
};

enum SCANERSTATE
{
    setScanerOff = 0,
    setScanerOn = 1
};

enum ENABLEACMESSAGE
{
    ACDISABLED = 0,
    ACRAWDATA = 1,
    ACSTATE = 2
};

typedef unsigned short tACThLine[NumberOfReceivers][3];    // один элемент для 2-х сторон

#define AContactZone 50 // длительность участка после развертки для подсчета суммы АК в мС
#define MaxAScanDuration (255 - AContactZone)

#define cSpeedCalcTime 500 // для расчета скорости - числа отсчетов основного ДП

#endif
