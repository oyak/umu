#ifndef TROLLEY_H
#define TROLLEY_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QVector>
#include "CriticalSection_Lin.h"
#include "test.h"


#define AbsMaxV 1.38 // модуль максимальной разрешенной скорости контроля, мм/мс

typedef struct
{
    unsigned int Time;
    int StartCoord;
    int StartCoordL;
    int StartCoordR;
    double TargetSpeed;
} tMovingTarget;


class TROLLEY : public QObject
{
    Q_OBJECT
public:
    TROLLEY(cCriticalSection *cs);
    ~TROLLEY();

    static const float step;
    static const unsigned int _timerPeriod;
    static const double _absDiscrepancyMaxOfCoord; // максимальное значение невязки по координате в мм

    void changeMovingParameters(float targetSpeed, int coord, int coordL, int coordR, unsigned int time);
    void setCoordinate(int coord, int coordL, int coordR);
    void stopTrolley();
    void setMovingDirection(Test::eMovingDir movingDirection);



signals:
    void pathStep(int shift, int coordinateLInMM, int coordinateRInMM);

public slots:
    void proc1ms();

private:

    int _stepCoordinate;
    int _startStepCoordinate;
    double _coordinate;
    double _memCoordinate;
    double _memRotationDegree;
    double _rotationCoefficient; // рассчитываем в момент прихода данных от ПК (_targetRotationDegree - _rotationDegree) / (_targetCoordinate - _coordinate)
//                               // _memCoordinate = _coordinate
//  rotationDegree = (_coordinate - _memCoordinate) * _rotationCoefficient + _memRotationDegree
    double _rotationDegree; //  _coordinateL - _coordinateR
    double _lastCoordinate;

    double _targetCoordinate; // координата из сообщения NextTrackCoordinateId
//
    double _targetRotationDegree; //  _targetCoordinateL - _targetCoordinateR

    double _currentV; // текущая скорость, мм/мс
    double _targetV;
//    double _lastCoordDiscrepancy; // невязка по координате на прошлом проходе proc1ms()

    double _extrapolationTime; // 1/4 от среднего
// интервала между сообщениями NextTrackCoordinateId. мс - берем 12 мс
    Test::eMovingDir _movingDirection; // направление движения:


    enum eMovingState
    {
        Normal = 0, // _currentV == _targetV
        Correction = 1, // _currentV != _targetV
        Stopping = 2  // _targetV == 0, _currentV != 0
    };

    eMovingState _movingState;
    int _correctionCounter; // счет интервалов по 1 мс удержания скорректированной скорости.
//    после обнуления устанавливаем _currentV = _targetV
    int _correctionCounterInit; // начальное значение _correctionCounter
//
    QVector<tMovingTarget> _targets;

//
    cCriticalSection *_cs;
    QTimer _trolleyTimer1ms;

    void stopMoving();
    void ifPathEvent();
    float getDisplacement(float v0, float a, unsigned int time);

    unsigned int getCurrentTime(bool withCorrection);
    void setCoordinate(double coord, double coordL, double coordR);
    void setTrolleyTargetRotation(double targetCoordL, double targetCoordR);
    double VCalculate(double currentCoordinate, double targetCoordinate, double timePeriod);
};


#endif // TROLLEY_H




//

