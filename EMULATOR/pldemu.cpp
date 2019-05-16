#include <assert.h>
#include <QDebug>
#include "pldemu.h"
#include "MISC46_2.H"

PLDEMULATOR::PLDEMULATOR(cCriticalSection *cs): _cycledAScan(false),
                             _AScanReady(false),
                             _numOfTacts(1),
                             _AScanLine(0),
                             _started(false),
                             _RAMAccessed(false),
                             _AscanTact(0),
                             _BScanLine(0),
                             _DPShift(0)
{
    _cs = cs;
    memset(_AScanBuffer, 0, sizeof(_AScanBuffer));
    memset(reinterpret_cast<unsigned char*>(_BScanBuffer), 0, sizeof(_BScanBuffer));
    _pAScanTimer = nullptr;
    connect(this, SIGNAL(_AScanStarted(void)), this, SLOT(_onAScanStarted(void)));

    _pPulsePict = new ASCANPULSE(_AMaxAmplOffset);

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

bool PLDEMULATOR::addBScanSignal(unsigned int tact, unsigned int line, unsigned int delayMS, unsigned int delayFrac, unsigned int ampl)
{
    assert((line == 0) || (line == 1));
unsigned int numOfSignals = _BScanBuffer[line][tact][0].SignalCount;
bool res = true;

    _cs->Enter();
    if (numOfSignals < MaxNumOfSignals)
    {
        _BScanBuffer[line][tact][numOfSignals].MaximumAmpl = ampl;
        _BScanBuffer[line][tact][numOfSignals].MaximumDelay = delayMS;
        _BScanBuffer[line][tact][numOfSignals].MaxAndOutsetTFrac = (unsigned char)((delayFrac & 0xFF) << 4);

        _BScanBuffer[line][tact][0].SignalCount++;
    }
        else res = false;
    _cs->Release();
    return res;
}

void PLDEMULATOR::resetSignals(unsigned char tact, unsigned int line)
{
    assert((line == 0) || (line == 1));
    _cs->Enter();
    _BScanBuffer[line][tact][0].SignalCount = 0;
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
                 else if( (regAddress >= BScanASD_0) && ((regAddress >> 1) <= ( (BScanASD_0 >> 1) + sizeof(tMaximumParameters) * MaxNumOfTacts * MaxNumOfSignals) ))
                      {
                         unsigned char *pUCHAR = (unsigned char*)&_BScanBuffer[_BScanLine];
                         res = pUCHAR[(regAddress - BScanASD_0) >> 1];
                      }
                          else if (regAddress == LENGTHDPVALUEADR)
                               {
                                   res = _DPShift;
                                   if (_DPShift)
                                   {
                                   _DPShift = 0;
                                   }
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
        _RAMAccessed = (regValue & 0x01) ? true:false;
    }
        else if (regAddress == a0x1301)
             {
                 assert(regValue <= MaxNumOfTacts);
                 assert(_started == false);
                 _numOfTacts = regValue;
             }
                 else
                 {
                     if (!_RAMAccessed)
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
                                                       assert(_started);
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
                                                                 if (regValue == 2) _BScanLine = 0;
                                                                     else if (regValue == 3) _BScanLine = 1;
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
