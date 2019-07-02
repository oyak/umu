#include <assert.h>
#include <QDebug>
#include "pldemu.h"
#include "math.h"
#include "MISC46_2.H"

PLDEMULATOR::PLDEMULATOR(cCriticalSection *cs): _cycledAScan(false),
                             _AScanReady(false),
                             _numOfTacts(1),
                             _AScanLine(0),
                             _started(false),
                             _RAMAccessible(false),
                             _AScanTact(0),
                             _BScanLine(0),
                             _DPShift(0),
                             _ASDBufferAccessible(false)
{
    _cs = cs;
    memset(_AScanBuffer, 0, sizeof(_AScanBuffer));
    memset(reinterpret_cast<unsigned char*>(_BScanBuffer), 0, sizeof(_BScanBuffer));
    memset (reinterpret_cast<unsigned char*>(_strobLimits), 0, sizeof(_strobLimits));
    memset (reinterpret_cast<unsigned char*>(_ASDBuffer), 0, sizeof(_ASDBuffer));
    _pAScanTimer = nullptr;
    connect(this, SIGNAL(_AScanStarted(void)), this, SLOT(_onAScanStarted(void)));

    _pPulsePict = new ASCANPULSE(AMaxAmplOffset);
    _tactParameterAreaSize = parreg_sz * MaxNumOfTacts;

// таблица перевода кода амплитуды в децибелы относительно порога 32
    _amplInDB[0] = -12;
    _amplInDB[1] = -10;
    _amplInDB[2] = -8;
    _amplInDB[3] = -6;
    _amplInDB[4] = -4;
    _amplInDB[5] = -2;
    _amplInDB[6] = 0;
    _amplInDB[7] = 2;
    _amplInDB[8] = 4;
    _amplInDB[9] = 6;
    _amplInDB[10] = 8;
    _amplInDB[11] = 10;
    _amplInDB[12] = 12;
    _amplInDB[13] = 14;
    _amplInDB[14] = 16;
    _amplInDB[15] = 18;


    // таблица перевода кодирования дБ относительно порога в 32
    // отсчета  в амплитуду
    for (int ii = 0; ii < (int) sizeof(_ampl); ++ii) {
        float bell = (ii - 12) / 20.0;
        _ampl[ii] = (unsigned char) (pow(10.0, bell) * 32.0);
    }

/*
    for(int ii = 0; ii < MaxNumOfTacts; ++ii)
    {
        _BScanBuffer[0][ii][0].SignalCount = 1;
        _BScanBuffer[0][ii][0].MaximumDelay = (ii + 1) * 10;
        _BScanBuffer[0][ii][0].MaximumAmpl = (ii + 1) * 20;
    }
*/
}

PLDEMULATOR::~PLDEMULATOR()
{
    delete _pPulsePict;
    if (_pAScanTimer != nullptr)
    {
        _pAScanTimer->stop();
        delete _pAScanTimer;
    }

}
#include <QTime>

void PLDEMULATOR::constructAScan(bool needBlocked)
{
unsigned char offsetOfCenterSignal;
int indexOfBScanSignal;

    assert((_AScanLine == 0) || (_AScanLine == 1));
    assert(_AScanTact < MaxNumOfTacts);
    if (needBlocked) _cs->Enter();

//    for (int ii = QTime::currentTime().msecsSinceStartOfDay() & 0xFF; ii < _AMaxAmplOffset; ++ii)
//        _AScanBuffer[ii] = ii;

    memset(_AScanBuffer, 0, sizeof(_AScanBuffer));
    if (_BScanBuffer[_AScanLine][_AScanTact][0].SignalCount)
    {
       for (unsigned int ii=0; ii<_BScanBuffer[_AScanLine][_AScanTact][0].SignalCount; ++ii)
       {
       unsigned int amplitude = _BScanBuffer[_AScanLine][_AScanTact][ii].MaximumAmpl; // + _pPulsePict->getRandUcharByIndexes(++_randomSampleIndex, 0);
//        if (amplitude > 255) amplitude = 255;
          offsetOfCenterSignal = timeToAScanBufferOffset(_BScanBuffer[_AScanLine][_AScanTact][ii].MaximumDelay, _BScanBuffer[_AScanLine][_AScanTact][ii].MaxAndOutsetTFrac >> 4, _AScanScale);
           _pPulsePict->drawSample(_AScanBuffer, offsetOfCenterSignal, _AScanScale, amplitude);
       }
    }
    for (unsigned int ii=0; ii < AMaxAmplOffset; ++ii)
    {
        if (_AScanBuffer[ii] == 0) _AScanBuffer[ii] = _pPulsePict->getRandUcharByIndexes(++_randomSampleIndex, ii);
    }

    indexOfBScanSignal = findMaxBScanSignal(_AScanTact, _AScanLine, _AScanStrobForMax);

    if (indexOfBScanSignal >= 0)
    {
        _AScanBuffer[AMaxAmplOffset] = _BScanBuffer[_AScanLine][_AScanTact][indexOfBScanSignal].MaximumAmpl;
        _AScanBuffer[AMaxDelayOffset] = _BScanBuffer[_AScanLine][_AScanTact][indexOfBScanSignal].MaximumDelay;
        _AScanBuffer[AMaxDelayFracOffset] = _BScanBuffer[_AScanLine][_AScanTact][indexOfBScanSignal].MaxAndOutsetTFrac >> 4;

//        qDebug() << "Label data: Delay = " <<  _AScanBuffer[AMaxDelayOffset] << "Amplitude = " << _AScanBuffer[AMaxAmplOffset];

    }
        else
        {
            _AScanBuffer[AMaxAmplOffset] = 0;
            _AScanBuffer[AMaxDelayOffset] = 0;
            _AScanBuffer[AMaxDelayFracOffset] = 0;
        }
    if (needBlocked) _cs->Release();
}

bool PLDEMULATOR::addBScanSignal(unsigned int tact, unsigned int line, unsigned int delayMS, unsigned int delayFrac, unsigned int amplitudeCode)
{
    assert((line == 0) || (line == 1));
unsigned int numOfSignals = _BScanBuffer[line][tact][0].SignalCount;
bool res = true;

    _cs->Enter();

    if (numOfSignals < MaxNumOfSignals)
    {
        int amplitudeInDB = codeToDB(amplitudeCode);
        int correctedAmplitudeInDB = amplitudeInDB;
//        int correctedAmplitudeInDB = amplitudeInDB + getATTValueChange(tact, line, delayMS);

        _BScanBuffer[line][tact][numOfSignals].MaximumAmpl = DBToAmplitude(correctedAmplitudeInDB);
        _BScanBuffer[line][tact][numOfSignals].Reserved2 = amplitudeInDB;
        _BScanBuffer[line][tact][numOfSignals].MaximumDelay = delayMS;
        _BScanBuffer[line][tact][numOfSignals].MaxAndOutsetTFrac = (unsigned char)((delayFrac & 0xFF) << 4);
        _BScanBuffer[line][tact][0].SignalCount++;
    }
        else res = false;
//
    for (unsigned int strob=0; strob<MaxNumOfStrobs; ++strob)
    {
        if (isInstantInStrob(delayMS, tact, strob, line))
        {
            unsigned char a = 1 << strob;
            if (line) a <<= 4;
            _ASDBuffer[tact] |= a;

//            qWarning() << "_ASDBuffer: tact =" << tact << "data =" << _ASDBuffer[tact];
        }
    }
//
    _cs->Release();
    return res;
}

void PLDEMULATOR::redefineSignalAmplitudes()
{
    redefineSignalAmplitudes(0);
    redefineSignalAmplitudes(1);
}

void PLDEMULATOR::redefineSignalAmplitudes(unsigned int line)
{
    int correctedAmplitudeInDB;
    for(int tact=0; tact < _numOfTacts; ++tact)
    {
        if (_BScanBuffer[line][tact][0].SignalCount == 0) continue;
        for(int s=0; s < _BScanBuffer[line][tact][0].SignalCount; ++s)
        {
           correctedAmplitudeInDB = _BScanBuffer[line][tact][s].Reserved2 + getATTValueChange(tact, line, _BScanBuffer[line][tact][s].MaximumDelay);
           _BScanBuffer[line][tact][s].MaximumAmpl = DBToAmplitude(correctedAmplitudeInDB);
        }
    }
}

void PLDEMULATOR::resetSignals(unsigned char tact, unsigned int line)
{
    assert((line == 0) || (line == 1));
    _cs->Enter();
    _BScanBuffer[line][tact][0].SignalCount = 0;

    if (line == 0) _ASDBuffer[tact] &= 0xF0;
        else _ASDBuffer[tact] &= 0x0F;

    _cs->Release();
}
//
unsigned char PLDEMULATOR::readRegister(unsigned int regAddress)
{
unsigned char res = 0;
unsigned int regAddressMasked = regAddress & ~LineMaskForAScan;

    assert((_AScanLine == 0) || (_AScanLine == 1));
    _cs->Enter();
    if (regAddressMasked ==  Aaddr1 + (0xFF<<1))
    {
        res = 0x00; //(_AScanReady == true) ? 0x00:0x01;
    }
        else if (regAddressMasked >= Aaddr1)
             {
                 if  (regAddressMasked <=  Aaddr1 + (AMaxDelayFracOffset<<1))
                 {
                     res = _AScanBuffer[(regAddressMasked >> 1) & 0xFF];
                 }
             }
                 else if ( (!_ASDBufferAccessible) && (regAddress >= BScanASD_0) && ((regAddress >> 1) <= ( (BScanASD_0 >> 1) + sizeof(tMaximumParameters) * MaxNumOfTacts * MaxNumOfSignals) ))
                      {
                         unsigned char *pUCHAR = (unsigned char*)&_BScanBuffer[_BScanLine];
                         res = pUCHAR[(regAddress - BScanASD_0) >> 1];
                      }
                          else if((_ASDBufferAccessible) && (regAddress >= BScanASD_0) && ( ((regAddress >> 1) < (BScanASD_0 >> 1) + MaxNumOfTacts )) )
                               {
                                   res = _ASDBuffer[(regAddress - BScanASD_0) >> 1];

//                                   if (res)
//                                       qWarning() << "readRegister: _ASDBuffer - Offset =" << ((regAddress - BScanASD_0) >> 1) << "Data =" << res;
                               }
                                   else if (regAddress == LENGTHDPVALUEADR)
                                        {
                                            res = _DPShift;
                                            _DPShift = 0;
                                        }
                                            else if (regAddress == LENGTHDPVALUEADR)
                                                 {
                                                     res = 0;
                                                 }

    _cs->Release();
    return res;
}
void PLDEMULATOR::writeRegister(unsigned int regAddress, unsigned char regValue)
{
    _cs->Enter();
    if (regAddress == a0x1300)
    {
        _started = (regValue & 0x02) ? true:false;
        _RAMAccessible = (regValue & 0x01) ? true:false;
    }
        else if (regAddress == a0x1301)
             {
                 assert(regValue <= MaxNumOfTacts);
                 assert(_started == false);
                 _numOfTacts = regValue + 1; // в ПЛИС записываем максимальный номер такта
                 _tactParameterAreaSize = parreg_sz * _numOfTacts;
                 for(int ii=0; ii< TACT_WORK_AREA_SIZE; ++ii)
                 {
                     _workAreaInital[ii] = true;
                 }
             }
                 else
                 {
                     if (!_RAMAccessible)
                     {
                     unsigned int regAddressMasked = regAddress & ~LineMaskForAScan;
                         if (regAddressMasked ==  Aaddr1 + (0xFE<<1))
                         {
                             _AScanScale = regValue;
                         }
                            else if (regAddressMasked ==  Aaddr1 + (0xFD<<1))
                                 {
                                     _AScanStart = regValue;
                                 }
                                     else if (regAddressMasked ==  Aaddr1 + (0xFC<<1))
                                          {
                                              assert((regValue & tactbitmsk) < MaxNumOfTacts);
                                              _AScanTact = regValue & tactbitmsk;
                                              _AScanLine = (regValue & 0x80) ? 1:0;
                                          }
                                              else if (regAddressMasked ==  Aaddr1 + (0xFF<<1))
                                                   {
//                                                       assert(_started);
                                                       _AScanStrobForMax = (regValue >> 2) & 0x3;
                                                       _AScanReady = false;

/*
                                                       if (_pAScanTimer == nullptr)
                                                       {
                                                           _pAScanTimer = new QTimer;
                                                           _pAScanTimer->setInterval(1);
                                                           connect(_pAScanTimer, SIGNAL(timeout()), this, SLOT(_onAscanTimer()));
                                                       }
*/
                                                       emit _AScanStarted();
                                                   }
                                                       else  if(regAddress ==  BScanASDCRegAddr)
                                                             {
                                                                 if ((regValue & 0x7F) == 0)
                                                                 {
                                                                     _ASDBufferAccessible = true;
                                                                 }
                                                                     else
                                                                     {
                                                                         _ASDBufferAccessible = false;
                                                                         if (regValue == 2) _BScanLine = 0;
                                                                             else if (regValue == 3) _BScanLine = 1;
                                                                     }
                                                             }

                    }
                 }
    _cs->Release();
}

unsigned char PLDEMULATOR::timeToAScanBufferOffset(unsigned char timeUs, unsigned char timeFrac, unsigned char scale)
{
unsigned int res;

    res = (timeUs * 10 + timeFrac) / scale;
    assert(res && (res < 255));
    return (unsigned char)res;
}

// сигнал о запуске А-развертки
void PLDEMULATOR::_onAScanStarted(void)
{
//    _pAScanTimer->start();
    _onAscanTimer();

}

// слот на срабатывание таймера _AScanTimer
void PLDEMULATOR::_onAscanTimer()
{
    _cs->Enter();
//    _pAScanTimer->stop();

    if (_AScanScale != 0)
    {
       constructAScan(false);
       _AScanReady = true;
    }
    _cs->Release();
}

void PLDEMULATOR::setPathShft(int shift)
{
    _cs->Enter();
    _DPShift = shift;
    _cs->Release();
}

unsigned int PLDEMULATOR::getNumberOfTacts()
{
    return static_cast<unsigned int>(_numOfTacts);
}

void PLDEMULATOR::writeIntoRAM(unsigned int address, unsigned char value)
{

    assert (_RAMAccessible);
    assert (_numOfTacts);
    _cs->Enter();
    address -= ExtRamStartAdr;
    assert((address & 0x8000) == 0);
    if (address < (unsigned int)_tactParameterAreaSize)
    {
        _tactParameterArea[address >> 1] = value;
//        qDebug() << "writeIntoRAM: address =" << hex << address + ExtRamStartAdr << "value =" << value << "into _tactParameterArea, offset = " << address;
    }
        else
        {
            if (_workAreaInital[(address - (unsigned int)_tactParameterAreaSize) >> 1])
            {
                _tactWorkAreaInital[(address - (unsigned int)_tactParameterAreaSize) >> 1] = value;
                _workAreaInital[(address - _tactParameterAreaSize) >> 1] = false;
            }
            _tactWorkArea[(address - _tactParameterAreaSize) >> 1] = value;

//            qDebug() << "writeIntoRAM: address =" << hex << address + ExtRamStartAdr << "value =" << value << "into _tactWorkArea, offset = " << ((address - _tactParameterAreaSize) >> 1);

            defineStrobsLimits();
//            redefineSignalAmplitudes();
        }
    _cs->Release();
}

unsigned char PLDEMULATOR::readFromRAM(unsigned int address)
{
unsigned char res;

    assert (_RAMAccessible);
    assert (_numOfTacts);

    _cs->Enter();
    address -= ExtRamStartAdr;
    if (address < (unsigned int)_tactParameterAreaSize)
    {
        res = _tactParameterArea[address >> 1];
    }
        else
        {
            res = _tactWorkArea[(address - (unsigned int)_tactParameterAreaSize) >> 1];
        }
    _cs->Release();
    return res;
}

unsigned short PLDEMULATOR::getTactWorkAreaOffset(unsigned int tact)
{
unsigned short res;
    res = (_tactParameterArea[(parreg_sz >> 1)  * tact + (_WSALmask >> 1)+1] << 8);
    res |=  _tactParameterArea[(parreg_sz >> 1) * tact + (_WSALmask >> 1)];
    if (res >= ExtRamStartAdr + _tactParameterAreaSize)
    {
        res -= (ExtRamStartAdr + _tactParameterAreaSize);
        res >>= 1;
    }
        else res = 0;
    return res;
}

// возвращает разницу начального и текущего положения аттенюатора в дБ
// для линии line и времени timeUs
int PLDEMULATOR::getATTValueChange(unsigned int tactNumber, unsigned char line, unsigned char timeUS)
{
tTactWorkAreaElement *pElementInital = reinterpret_cast<tTactWorkAreaElement*>(&_tactWorkAreaInital[getTactWorkAreaOffset(tactNumber)]);
tTactWorkAreaElement *pElement = reinterpret_cast<tTactWorkAreaElement*>(&_tactWorkArea[getTactWorkAreaOffset(tactNumber)]);

    if (line == 0)
    {
        return (pElement[timeUS].DACValueLine0 - pElementInital[timeUS].DACValueLine0) / 2;
//        return (pElement->DACValueLine0 - pElementInital->DACValueLine0) / 2;
    }
    return (pElement[timeUS].DACValueLine1 - pElementInital[timeUS].DACValueLine1) / 2;
//    return (pElement->DACValueLine1 - pElementInital->DACValueLine1) / 2;
}

void PLDEMULATOR::defineStrobLimits(unsigned int tactNumber, unsigned int strobNumber, unsigned int line)
{
unsigned int stage;
tTactWorkAreaElement *pElement = reinterpret_cast<tTactWorkAreaElement*>(&_tactWorkArea[getTactWorkAreaOffset(tactNumber)]);
unsigned char byteMask;

    if (line == 0)
    {
        byteMask = mask[(strobNumber & 0x03)];
    }
        else
        {
            byteMask = mask[(strobNumber & 0x03) | 0x04];
        }

    stage = 0;
    for(unsigned int ii= 0; ii <= SAMPLE_DURATION; ++ii)
    {
        if (stage == 0)
        {
            if(pElement->StrobsMark & byteMask)
            {
                _strobLimits[line][tactNumber][strobNumber].Start = ii;
                stage = 1;
            }
        }
            else
            {
                if((pElement->StrobsMark & byteMask) == 0)
                {
                    if (ii)
                    {
                        _strobLimits[line][tactNumber][strobNumber].End = ii - 1;
                    }
                        else
                        {
                            _strobLimits[line][tactNumber][strobNumber].End = 0;
                        }
                    break;
                }
            }
            ++pElement;
    }
}

void PLDEMULATOR::defineStrobsLimits()
{
    for(unsigned int tact=0; tact < _numOfTacts; ++tact)
        for(unsigned int strobNum=0; strobNum < MaxNumOfStrobs; ++strobNum)
        {
            defineStrobLimits(tact, strobNum, 0); // линия 0
            defineStrobLimits(tact, strobNum, 1);
        }
}

bool PLDEMULATOR::isInstantInStrob(unsigned char timeUS, unsigned int tactNumber, unsigned int strobNumber, unsigned int line)
{
    return ((timeUS >= _strobLimits[line][tactNumber][strobNumber].Start) && (timeUS <= _strobLimits[line][tactNumber][strobNumber].End));
}
//
// находит в сигналах B-развертки для заданного такта максимальный сигнал(его индекс), лежащий в пределах
// заданного строба
// если сигналов в стробе нет - возвращает -1
int PLDEMULATOR::findMaxBScanSignal(unsigned int tact, unsigned line, unsigned int strob)
{
unsigned int currentMaxAmplitude;
unsigned int signalCount;
int res = -1;

    if (_BScanBuffer[line][tact][0].SignalCount == 0) return -1;

    for(signalCount=0; signalCount < _BScanBuffer[line][tact][0].SignalCount; ++signalCount)
    {
        if(isInstantInStrob(_BScanBuffer[line][tact][signalCount].MaximumDelay, tact, strob, line))
        {
            currentMaxAmplitude = _BScanBuffer[line][tact][signalCount].MaximumAmpl;
            break;
        }
    }
    if (signalCount == _BScanBuffer[line][tact][0].SignalCount) return -1; // нет сигналов в стробе
    if (_BScanBuffer[line][tact][0].SignalCount == 1) return signalCount;
    if (signalCount < _BScanBuffer[line][tact][0].SignalCount - 1)
    {
        for(unsigned int ii=signalCount+1; ii < _BScanBuffer[line][tact][0].SignalCount; ++ii)
        {
            if ((currentMaxAmplitude < _BScanBuffer[line][tact][ii].MaximumAmpl) &&
                isInstantInStrob(_BScanBuffer[line][tact][signalCount].MaximumDelay, tact, strob, line))
            {
                currentMaxAmplitude = _BScanBuffer[line][tact][ii].MaximumAmpl;
                signalCount = ii;
            }
        }
       return signalCount;
   }
   return -1;
}
