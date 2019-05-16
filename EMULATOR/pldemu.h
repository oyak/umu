#ifndef PLDEMU_H
#define PLDEMU_H

#include <QTimer>
#include <QObject>
#include "CriticalSection.h"
#include "ustyp468.h"
#include "ascanpulse.h"

const unsigned int PLDVerId = 0x2048; // версия эмулируемой прошивки ПЛИС

#define MaxNumOfTacts 8
#define MaxNumOfSignals 8

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

signals:
    void _AScanStarted(void);


public slots:
    void _onAscanTimer();
    void _onAScanStarted(void);


private:
    bool _started; // автомат PLD работает
    bool _RAMAccessed; // для контроллера доступна микросхема ОЗУ
    unsigned char _numOfTacts; // установленное число тактов

    tMaximumParameters _BScanBuffer[2][MaxNumOfTacts][MaxNumOfSignals]; // линия-такт-сигналы
    unsigned char _BScanLine;
//
    class ASCANPULSE *_pPulsePict;

    unsigned char _AScanBuffer[255];
    QTimer *_pAScanTimer;
    unsigned char _AScanLine;
    unsigned char _AscanTact;
    unsigned char _AscanScale;
    unsigned char _AscanStart;
//
    unsigned char _AscanStrobForMax;

    bool _cycledAScan;
    bool _AScanReady;
    unsigned char _randomSampleIndex;  // UCHAR !


    unsigned char _DPShift; //

    cCriticalSection* _cs;


    void constructAScan(bool needBlocked);
    unsigned char timeToAScanBufferOffset(unsigned char timeUs, unsigned char timeFrac, unsigned char scale);
};


#endif // PLDEMU_H
