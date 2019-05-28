#ifndef PLDEMU_H
#define PLDEMU_H

#include <QTimer>
#include <QObject>
#include <assert.h>
#include "CriticalSection.h"
#include "ustyp468.h"
#include "ascanpulse.h"

extern const UCHAR mask[8];

const unsigned int PLDVerId = 0x2048; // верси€ эмулируемой прошивки ѕЋ»—

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

//  смещени€ данных о найденном максимуме ј развертки в _AScanBuffer
#define AMaxAmplOffset 0xE8
#define AMaxDelayOffset 0xE9
#define AMaxDelayFracOffset 0xEA

#pragma pack(push, 1)

typedef struct
{
    unsigned char Reserved1;
    unsigned char DACValueLine0;
    unsigned char DACValueLine1;
    unsigned char StrobsMark; // бит 0 - соответствует стробу 0 и т.д.
} tTactWorkAreaElement; // элемент рабочей области тактов, соответствующий каждой микросекунде развертки

#define SAMPLE_DURATION 255

//#define TACT_PARAMETERS_AREA_SIZE 256

#define TACT_WORK_AREA_SIZE ((SAMPLE_DURATION + 1) * sizeof(tTactWorkAreaElement) * MaxNumOfTacts)

typedef struct
{
    unsigned char SignalCount;
    unsigned char OutsetAmpl;
    unsigned char MaximumAmpl;
    unsigned char OutsetDelay;
    unsigned char MaximumDelay;
    unsigned char EndDelay;
    unsigned char MaxAndOutsetTFrac; // MaxTFrac - старша€ тетрада
    unsigned char Reserved2; // это поле используем дл€ сохранени€ амплитуды максимума, полученного изначально
// в виде кода из объекта сигналов и преобразованного в dB
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
    bool addBScanSignal(unsigned int tact, unsigned int line, unsigned int delayMS,  unsigned int delayFrac, unsigned int amplInDB);
    unsigned int getNumberOfTacts();
    void setPathShft(int shift);

    unsigned char readRegister(unsigned int regAddress);
    void writeRegister(unsigned int regAddress, unsigned char regValue);

    void writeIntoRAM(unsigned int address, unsigned char value);
    unsigned char readFromRAM(unsigned int address);
    int getATTValueChange(unsigned int tactNumber, unsigned char line, unsigned char timeUS);


signals:
    void _AScanStarted(void);


public slots:
    void _onAscanTimer();
    void _onAScanStarted(void);


private:
    bool _started; // автомат PLD работает
    bool _RAMAccessible; // дл€ контроллера доступна микросхема ќ«”
    unsigned char _numOfTacts; // установленное число тактов
    unsigned short _tactParameterAreaSize; // текущий размер области параметров тактов, зависит от числа установленных тактов

    tMaximumParameters _BScanBuffer[2][MaxNumOfTacts][MaxNumOfSignals]; // лини€-такт-сигналы
    unsigned char _BScanLine;
//
    class ASCANPULSE *_pPulsePict;

    unsigned char _AScanBuffer[SAMPLE_DURATION];
    QTimer *_pAScanTimer;
    unsigned char _AScanLine;
    unsigned char _AScanTact;
    unsigned char _AScanScale;
    unsigned char _AScanStart;
//
    unsigned char _AScanStrobForMax;

    bool _cycledAScan;
    bool _AScanReady;

// буфер ј—ƒ - в каждом байте - мл.тетрада - данные линии 0, ст.тетрада - линии 1
// в пределах тетрады: мл.бит - сигналы в стробе 0, ст.бит - присутствуют сигналы в стробе 3
    unsigned char _ASDBuffer[MaxNumOfTacts];
    bool _ASDBufferAccessible;

    tStrobLimit _strobLimits[2][MaxNumOfTacts][MaxNumOfStrobs];

    unsigned char _randomSampleIndex;  // UCHAR !


    unsigned char _DPShift; //

    cCriticalSection* _cs;
    int _amplInDB[16];
    int _ampl[31];


    bool _workAreaInital[TACT_WORK_AREA_SIZE];
    unsigned char _tactParameterArea[parreg_sz * MaxNumOfTacts];
    unsigned char _tactWorkAreaInital[TACT_WORK_AREA_SIZE]; // начальные значени€, записываемые в область
    // параметров такта после установки числа тактов
    unsigned char _tactWorkArea[TACT_WORK_AREA_SIZE];

    unsigned char codeToDB(unsigned int code)
    {
        return _amplInDB[code & 0xF];
    }

    unsigned char DBToAmplitude(int dB)
    {
        if (dB < -12) dB = -12;
            else
            {
                if (dB > 18) dB = 18;
            }
        return _ampl[dB + 12];
    }

    void constructAScan(bool needBlocked);
    unsigned char timeToAScanBufferOffset(unsigned char timeUs, unsigned char timeFrac, unsigned char scale);
    void defineStrobLimits(unsigned int tactNumber, unsigned int strobNumber, unsigned int line);
    void defineStrobsLimits();
    bool isInstantInStrob(unsigned char timeUS, unsigned int tactNumber, unsigned int strobNum, unsigned int line);
    int findMaxBScanSignal(unsigned int tact, unsigned line, unsigned int strob);

    unsigned short getTactWorkAreaOffset(unsigned int tact);
    void redefineSignalAmplitudes();
    void redefineSignalAmplitudes(unsigned int line);
};


#endif // PLDEMU_H
