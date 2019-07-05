#ifndef TROLLEY_H
#define TROLLEY_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QVector>
#include "CriticalSection_Lin.h"
#include "test.h"


#define AbsMaxV 1.389 // ������ ������������ ����������� �������� ��������, ��/��

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

    static const double step;
    static const unsigned int _timerPeriod;
    static const double _absDiscrepancyMaxOfCoord; // ������������ �������� ������� �� ���������� � ��

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
    double _rotationCoefficient; // ������������ � ������ ������� ������ �� �� (_targetRotationDegree - _rotationDegree) / (_targetCoordinate - _coordinate)
//                               // _memCoordinate = _coordinate
//  rotationDegree = (_coordinate - _memCoordinate) * _rotationCoefficient + _memRotationDegree
    double _rotationDegree; //  _coordinateL - _coordinateR
    double _lastCoordinate;

    double _targetCoordinate; // ���������� �� ��������� NextTrackCoordinateId
//
    double _targetRotationDegree; //  _targetCoordinateL - _targetCoordinateR

    double _currentV; // ������� ��������, ��/��
    double _targetV;
//    double _lastCoordDiscrepancy; // ������� �� ���������� �� ������� ������� proc1ms()

    double _extrapolationTime; // ����� �������� ������ ��������
// ��������� ����� ����������� NextTrackCoordinateId. ��
    Test::eMovingDir _movingDirection; // ����������� ��������:


    enum eMovingState
    {
        Normal = 0, // _currentV == _targetV
        Correction = 1, // _currentV != _targetV
        Stopping = 2  // _targetV == 0, _currentV != 0
    };

    eMovingState _movingState;
    int _correctionCounter; // ���� ���������� �� 1 �� ��������� ����������������� ��������.
//    ����� ��������� ������������� _currentV = _targetV
    int _correctionCounterInit; // ��������� �������� _correctionCounter
//
    QVector<tMovingTarget> _targets;

//
    cCriticalSection *_cs;
    QTimer _trolleyTimer1ms;
    unsigned int _timeSinceMidNight;

    void stopMoving();
    void ifPathEvent();
    float getDisplacement(float v0, float a, unsigned int time);

    unsigned int getCurrentTime() // �������� ������� ����� �� ������ �����
    {
     unsigned int res;
     QTime currentT = QTime::currentTime();
         res = currentT.msecsSinceStartOfDay();
         return res;
    }
    void setCoordinate(double coord, double coordL, double coordR);
    void setTrolleyTargetRotation(double targetCoordL, double targetCoordR);
    double VCalculate(double currentCoordinate, double targetCoordinate, double timePeriod);
    void changeCoordinate(unsigned int timeSpan)
    {
        for (unsigned int ii=0; ii < timeSpan; ++ii)
        {
            _coordinate += _currentV;
           if ((_currentV > 0.0) && (_coordinate < _targetCoordinate) || (_currentV < 0.0) && (_coordinate > _targetCoordinate))
           {
               _rotationDegree = _rotationCoefficient * (_coordinate - _memCoordinate) + _memRotationDegree;
           }
               else _rotationDegree = _targetRotationDegree;
          ifPathEvent();
        }
    };
};

#endif // TROLLEY_H

