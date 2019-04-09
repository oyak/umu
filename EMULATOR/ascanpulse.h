#ifndef ASCANPULSE_H
#define ASCANPULSE_H

#include <QVector>


enum eDurations
{
    Duration3 = 0,
    Duration6 = 1,
    Duration10 = 2
};
#define NumOfDurations 3

enum eDots
{
    Qdots11 = 4,
    Qdots19 = 3,
    Qdots25 = 2,
    Qdots33 = 1,
    Qdots59 = 0,
    QDotsVariant = 5
};

#define NumOfAmplitudes 250


#define SamplePeriodScale1 1e-7
#define SamplePeriodScale3 3.33e-7

#define Dots5 5
#define Dots7 7
#define Dots9 9
#define Dots11 11
#define Dots15 15
#define Dots19 19
#define Dots25 25
#define Dots29 29
#define Dots33 33
#define Dots59 59
#define Dots99 99


#define MinimalAmplitude 5

typedef QVector<unsigned char> tSample;

class ASCANPULSE
{
public:
    ASCANPULSE(unsigned int maxOffsetInAScanBuffer);
    ~ASCANPULSE();

    unsigned int getMinAmplitude()
    {
        return MinimalAmplitude;
    }

    unsigned int getMaxAmplitude()
    {
        return NumOfAmplitudes + getMinAmplitude();
    }

    void drawSample(unsigned char *AScanBuffer,  unsigned char offsetOfCenter, unsigned char scale, unsigned int amplitude);
    unsigned getRandUcharByIndexes(unsigned char index1, unsigned char index2)
    {
        return _randBuffer[index1][index2];
    }
private:
    tSample _sample[QDotsVariant][NumOfAmplitudes];
    unsigned char _maxOffsetInASCanBuffer;
    unsigned char _randBuffer[256][256];
    void defineSamples();
    void clearSamples();
    unsigned char getRandomNumber(int min, int max);
    unsigned int scaleToDotsIdx(unsigned char scale);
    unsigned int dotsToDotsIdx(int numOfDots);
    int idxToDots(unsigned int index);
};

#endif // ASCANPULSE_H
