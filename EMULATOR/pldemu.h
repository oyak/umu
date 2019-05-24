#ifndef PLDEMU_H
#define PLDEMU_H

#include <QTimer>
#include <QObject>
#include "CriticalSection.h"
#include "ustyp468.h"
#include "ascanpulse.h"

extern const UCHAR mask[8];

const unsigned int PLDVerId = 0x2048; // версия эмулируемой прошивки ПЛИС

#define MaxNumOfTacts 8
#define MaxNumOfSignals 8
#define MaxNumOfStrobs 4

//#define BScanASDBaseAddr 0x11
//#define AScanStartAddr1 0x20
//#define AScanStartAddr2 0x21

//#define  Aaddr1 (AScanStartAddr1<<9)
//#define  Aaddr2 (AScanStartAddr2<<9)

#define LineMaskForAScan (0x1 << 9)
#define AScanRegimMask (0xC) << 3)

#define BScanNumOfSignalsMask (0x3 << 6)

//#define a0x1300 (0x1300<<1)
//#define a0x1301 (0x1301<<1) // число тактов

//  смещения данных о найденном максимуме А развертки в _AScanBuffer
#define _AMaxAmplOffset 0xE8
#define _AMaxDelayOffset 0xE9
#define _AMaxDelayFracOffset 0xEA

#pragma pack(push, 1)

typedef struct
{
    unsigned char Reserved1;
    unsigned char DACValueLine0;
    unsigned char DACValueLine1;
    unsigned char StrobsMark; // бит 0 - соответствует стробу 0 и т.д.
} tTactWorkAreaElement; // элемент рабочей области тактов, соответствующий каждой микросекунде развертки

#define SAMPLE_DURATION 255

#define TACT_PARAMETERS_AREA_SIZE 256

#define TACT_WORK_AREA_SIZE ((SAMPLE_DURATION + 1) * sizeof(tTactWorkAreaElement) * MaxNumOfTacts)

typedef struct
{
    unsigned char SignalCount;
    unsigned char OutsetAmpl;
    unsigned char MaximumAmpl;
    unsigned char OutsetDelay;
    unsigned char MaximumDelay;
    unsigned char EndDelay;
    unsigned char MaxAndOutsetTFrac; // MaxTFrac - старшая тетрада
    unsigned char Reserved2;
} tMaximumParameters;

#pragma pack(pop)

typedef struct
{
    unsigned char Start;
    unsigned char End;

} tStrobLimit;

class PLDEMULATOR: public QObject
{
    Q_OBJECT
public:
    PLDEMULATOR(cCriticalSection* cs);
    ~PLDEMULATOR();
    void resetSignals(unsigned char tact, unsigned int line);
    bool addBScanSignal(unsigned int tact, unsigned int line, unsigned int delayMS,  unsigned int delayFrac, unsigned int ampl);
    unsigned int getNumberOfTacts();
    void setPathShft(int shift);

    unsigned char readRegister(unsigned int regAddress);
    void writeRegister(unsigned int regAddress, unsigned char regValue);

    void writeIntoRAM(unsigned short address, unsigned char value);
    unsigned char readFromRAM(unsigned char address);
    int getATTValueChange(unsigned char line, unsigned char timeUs);


signals:
    void _AScanStarted(void);


public slots:
    void _onAscanTimer();
    void _onAScanStarted(void);


private:
    bool _started; // автомат PLD работает
    bool _RAMAccessible; // для контроллера доступна микросхема ОЗУ
    unsigned char _numOfTacts; // установленное число тактов

    tMaximumParameters _BScanBuffer[2][MaxNumOfTacts][MaxNumOfSignals]; // линия-такт-сигналы
    unsigned char _BScanLine;
//
    class ASCANPULSE *_pPulsePict;

    unsigned char _AScanBuffer[SAMPLE_DURATION];
    QTimer *_pAScanTimer;
    unsigned char _AScanLine;
    unsigned char _AscanTact;
    unsigned char _AscanScale;
    unsigned char _AscanStart;
//
    unsigned char _AscanStrobForMax;

    bool _cycledAScan;
    bool _AScanReady;

// буфер АСД - в каждом байте - мл.тетрада - данные линии 0, ст.тетрада - линии 1
// в пределах тетрады: мл.бит - сигналы в стробе 0, ст.бит - присутствуют сигналы в стробе 3
    unsigned char _ASDBuffer[MaxNumOfTacts];
    bool _ASDBufferAccessible;

    tStrobLimit _strobLimits[2][MaxNumOfTacts][MaxNumOfStrobs];

    unsigned char _randomSampleIndex;  // UCHAR !


    unsigned char _DPShift; //

    cCriticalSection* _cs;

    bool _workAreaInital[TACT_WORK_AREA_SIZE];
    unsigned char _tactParameterArea[TACT_PARAMETERS_AREA_SIZE * MaxNumOfTacts];
    unsigned char _tactWorkAreaInital[TACT_WORK_AREA_SIZE]; // начальные значения, записываемые в область
    // параметров такта после установки числа тактов
    unsigned char _tactWorkArea[TACT_WORK_AREA_SIZE];

    void constructAScan(bool needBlocked);
    unsigned char timeToAScanBufferOffset(unsigned char timeUs, unsigned char timeFrac, unsigned char scale);
    void defineStrobLimits(unsigned int tactNumber, unsigned int strobNumber, unsigned int line);
    void defineStrobsLimits();
};


#endif // PLDEMU_H
