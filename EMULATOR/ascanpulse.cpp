#include <assert.h>
#include <math.h>

#include "ascanpulse.h"
#include "variety.h"


ASCANPULSE::ASCANPULSE(unsigned int maxOffsetInAScanBuffer)
{
    _maxOffsetInASCanBuffer = maxOffsetInAScanBuffer;
    defineSamples();
    for(int ii=0; ii < 256; ++ii)
        for(int jj=0; jj < 256; ++jj) _randBuffer[ii][jj] = getRandomNumber(0, 5);
}

ASCANPULSE::~ASCANPULSE()
{
    clearSamples();
}

//
unsigned int ASCANPULSE::scaleToDotsIdx(unsigned char scale)
{
    switch(scale)
    {
       case 1:
           return 0; // Dots33;
       case 3:
           return 1; // Dots19;
       case 4:
           return 2; //Dots11;
       case 6:
           return 3; //Dots7;
       case 8:
           return 4; // Dots5;
       default:
        assert(0); //
    }
    return 0;
}

unsigned int ASCANPULSE::dotsToDotsIdx(int numOfDots)
{
    switch(numOfDots)
    {
       case Dots33:
           return 0;
       case Dots19:
           return 1;
       case Dots11:
           return 2;
       case Dots7:
           return 3;
       case Dots5:
           return 4;
       default:
        assert(0); //
    }
    return 0;
}

int ASCANPULSE::idxToDots(unsigned int index)
{
    switch(index)
    {
       case 0:
           return Dots33;
       case 1:
           return Dots19;
       case 2:
           return Dots11;
       case 3:
           return Dots7;
       case 4:
           return Dots5;
       default:
        assert(0); //
    }
    return 0;
}

// значения копируются только в те точки, которые изначально равны 0
void ASCANPULSE::drawSample(unsigned char *AScanBuffer,  unsigned char offsetOfCenter, unsigned char scale, unsigned int amplitude)
{
    assert(amplitude >= getMinAmplitude());
register unsigned int ampIndex = amplitude -  getMinAmplitude();
register unsigned int offsetShiftOnLeft;
register int startOffsetInBuffer;
register int startOffsetInSample = 0;
int numOfDots;
unsigned int dotsIdx;

    dotsIdx = scaleToDotsIdx(scale);
    numOfDots = idxToDots(dotsIdx);

    offsetShiftOnLeft = numOfDots >> 1;
    startOffsetInBuffer = (unsigned int)offsetOfCenter - offsetShiftOnLeft;
    if (startOffsetInBuffer < 0)
    {
        startOffsetInSample -= startOffsetInBuffer;
        numOfDots -= startOffsetInBuffer;
        startOffsetInBuffer = 0;
    }
    tSample::iterator it = _sample[dotsIdx][ampIndex].begin() + startOffsetInSample;
    for(int ii=0; ii < numOfDots; ++ii)
    {
        assert(it != _sample[dotsIdx][ampIndex].end());
        if (AScanBuffer[startOffsetInBuffer] == 0) AScanBuffer[startOffsetInBuffer] = *it;
        ++startOffsetInBuffer;
        ++it;
    }
}

void ASCANPULSE::defineSamples()
{
unsigned int numOfSamples;
tSample s;

for (unsigned int kk=0; kk < QDotsVariant; ++kk)
{
   numOfSamples = idxToDots(kk);
   s.resize(numOfSamples);

   for (unsigned int jj=0; jj<NumOfAmplitudes; ++jj)
   {
       unsigned int ii = 0;
       for(tSample::iterator it = s.begin(); it < s.end(); ++it, ++ii)
           *it = (jj + MinimalAmplitude) * sin(3.141592 / numOfSamples * ii);
       _sample[kk][jj].resize(numOfSamples);
       _sample[kk][jj] = s;
   }
}
}

void ASCANPULSE::clearSamples()
{
    for (unsigned int ii=0; ii < QDotsVariant; ++ii)
    {
        for (unsigned int jj=0; jj<NumOfAmplitudes; ++jj)
        {
            _sample[ii][jj].clear();
        }
    }
}
