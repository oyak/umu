#ifndef TROLLEY_H
#define TROLLEY_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QVector>
#include "CriticalSection_Lin.h"
#include "test.h"



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
    static const double _absDiscrepancyMaxOfCoord; // ������������ �������� ������� �� ���������� � ��
    static const int _TimeLag; // ����������� ���������� ������������� ������� ����� ��������� ��������
// �� � �����

    void changeMovingParameters(float targetSpeed, int coord, int coordL, int coordR, unsigned int time);
    void setCoordinate(int coord, int coordL, int coordR);
    void stopTrolley();


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
    double _lastCoordDiscrepancy; // ������� �� ���������� �� ������� ������� proc1ms()

    int _timeCorrection; // �������� �������, ���� ����� ����� ���������

//
    QVector<tMovingTarget> _targets;

//
    cCriticalSection *_cs;
    QTimer _trolleyTimer1ms;

    void stopMoving();
    void ifPathEvent();
    float getDisplacement(float v0, float a, unsigned int time);

    unsigned int getCurrentTime(bool withCorrection);
    bool isTagetTimeCorrect(unsigned int timeByMS);
    void setCoordinate(double coord, double coordL, double coordR);
    void setTrolleyTargetRotation(double targetCoordL, double targetCoordR);
};


#endif // TROLLEY_H




//

