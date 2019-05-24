#include <assert.h>
#include <QDebug>
#include "pldemu.h"
#include "MISC46_2.H"

PLDEMULATOR::PLDEMULATOR(cCriticalSection *cs): _cycledAScan(false),
                             _AScanReady(false),
                             _numOfTacts(1),
                             _AScanLine(0),
                             _started(false),
                             _RAMAccessible(false),
                             _AscanTact(0),
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

    _pPulsePict = new ASCANPULSE(_AMaxAmplOffset);
    _tactParameterAreaSize = parreg_sz * MaxNumOfTacts;

    // таблица перевода кодирования дБ относительно порога в 32
    // отсчета  в амплитуду
    for (int ii = 0; ii < (int) sizeof(_ampl); ++ii) {
        float bell = (ii * 2 - 12.0) / 20.0;
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

    assert((_AScanLine == 0) || (_AScanLine == 1));
    assert(_AscanTact < MaxNumOfTacts);
    if (needBlocked) _cs->Enter();

//    for (int ii = QTime::currentTime().msecsSinceStartOfDay() & 0xFF; ii < _AMaxAmplOffset; ++ii)
//        _AScanBuffer[ii] = ii;


    memset(_AScanBuffer, 0, sizeof(_AScanBuffer));
    if (_BScanBuffer[_AScanLine][_AscanTact][0].SignalCount)
       for (unsigned int ii=0; ii<_BScanBuffer[_AScanLine][_AscanTact][0].SignalCount; ++ii)
       {
       unsigned int amplitude = _BScanBuffer[_AScanLine][_AscanTact][ii].MaximumAmpl; // + _pPulsePict->getRandUcharByIndexes(++_randomSampleIndex, 0);
//        if (amplitude > 255) amplitude = 255;
          offsetOfCenterSignal = timeToAScanBufferOffset(_BScanBuffer[_AScanLine][_AscanTact][ii].MaximumDelay, _BScanBuffer[_AScanLine][_AscanTact][ii].MaxAndOutsetTFrac >> 4, _AscanScale);
           _pPulsePict->drawSample(_AScanBuffer, offsetOfCenterSignal, _AscanScale, amplitude);
       }


    for (unsigned int ii=0; ii < _AMaxAmplOffset; ++ii)
    {
        if (_AScanBuffer[ii] == 0) _AScanBuffer[ii] = _pPulsePict->getRandUcharByIndexes(++_randomSampleIndex, ii);
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
        int correctedAmplitudeInDB = (int)amplitudeCode + getATTValueChange(tact, line, delayMS);
        if (correctedAmplitudeInDB < 0) correctedAmplitudeInDB = 0;
            else

        _BScanBuffer[line][tact][numOfSignals].MaximumAmpl = dBtoAmpl(correctedAmplitudeInDB);
        _BScanBuffer[line][tact][numOfSignals].MaximumDelay = delayMS;
        _BScanBuffer[line][tact][numOfSignals].MaxAndOutsetTFrac = (unsigned char)((delayFrac & 0xFF) << 4);
        _BScanBuffer[line][tact][0].SignalCount++;
    }
        else res = false;
//
    if (isInstantInStrob(delayMS, tact, 0, line))
    {
         unsigned char a = 1;
         if (line) a <<= 4;
        _ASDBuffer[tact] = a;
    }
//
    _cs->Release();
    return res;
}

void PLDEMULATOR::resetSignals(unsigned char tact, unsigned int line)
{
    assert((line == 0) || (line == 1));
    _cs->Enter();
    _BScanBuffer[line][tact][0].SignalCount = 0;
    memset (reinterpret_cast<unsigned char*>(_ASDBuffer), 0, sizeof(_ASDBuffer));
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
                 if  (regAddressMasked <=  Aaddr1 + (_AMaxDelayFracOffset<<1))
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
                             _AscanScale = regValue;
                         }
                            else if (regAddressMasked ==  Aaddr1 + (0xFD<<1))
                                 {
                                     _AscanStart = regValue;
                                 }
                                     else if (regAddressMasked ==  Aaddr1 + (0xFC<<1))
                                          {
                                              assert((regValue & tactbitmsk) < MaxNumOfTacts);
                                              _AscanTact = regValue & tactbitmsk;
                                              _AScanLine = (regValue & 0x80) ? 1:0;
                                          }
                                              else if (regAddressMasked ==  Aaddr1 + (0xFF<<1))
                                                   {
//                                                       assert(_started);
                                                       _AscanStrobForMax = (regValue >> 2) & 0x3;
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

    if (_AscanScale != 0)
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
    if (address < _tactParameterAreaSize)
    {
        _tactParameterArea[address >> 1] = value;
        qDebug() << "writeIntoRAM: address =" << hex << address + ExtRamStartAdr << "value =" << value << "into _tactParameterArea, offset = " << address;
    }
        else
        {
            if (_workAreaInital[(address - _tactParameterAreaSize) >> 1])
            {
                _tactWorkAreaInital[(address - _tactParameterAreaSize) >> 1] = value;
                _workAreaInital[(address - _tactParameterAreaSize) >> 1] = false;
            }
            _tactWorkArea[(address - _tactParameterAreaSize) >> 1] = value;

            qDebug() << "writeIntoRAM: address =" << hex << address + ExtRamStartAdr << "value =" << value << "into _tactWorkArea, offset = " << ((address - _tactParameterAreaSize) >> 1);

            defineStrobsLimits();
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
    if (address < _tactParameterAreaSize)
    {
        res = _tactParameterArea[address >> 1];
    }
        else
        {
            res = _tactWorkArea[(address - _tactParameterAreaSize) >> 1];
        }
    _cs->Release();
    return res;
}

// возвращает разницу начального и текущего положения аттенюатора в дБ
// для линии line и времени timeUs
int PLDEMULATOR::getATTValueChange(unsigned int tactNumber, unsigned char line, unsigned char timeUs)
{
tTactWorkAreaElement *pElementInital = reinterpret_cast<tTactWorkAreaElement*>(&_tactWorkAreaInital[tactNumber * (SAMPLE_DURATION + 1) + timeUs]);
tTactWorkAreaElement *pElement = reinterpret_cast<tTactWorkAreaElement*>(&_tactWorkArea[tactNumber * (SAMPLE_DURATION + 1) + timeUs]);
    if (line == 0)
    {
        return (pElement->DACValueLine0 - pElementInital->DACValueLine0) / 2;
    }
    return (pElement->DACValueLine1 - pElementInital->DACValueLine1) / 2;
}

void PLDEMULATOR::defineStrobLimits(unsigned int tactNumber, unsigned int strobNumber, unsigned int line)
{
unsigned int stage;
tTactWorkAreaElement *pElement = reinterpret_cast<tTactWorkAreaElement*>(&_tactWorkArea[tactNumber * (SAMPLE_DURATION + 1)]);
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
    return ((timeUs >= _strobLimits[line][tactNumber][strobNumber].Start) && (timeUs <= _strobLimits[line][tactNumber][strobNumber].End));
}
