#ifndef TROLLEY_H
#define TROLLEY_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QVector>
#include "CriticalSection_Lin.h"



typedef struct
{
    unsigned int Time;
    int StartCoord;
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
    static const int _TimeLug; // максимально допустимая положительная разница между системным временем
// ПК и здесь

    void changeMovingParameters(float targetSpeed, int coord, unsigned int time);
    void setCoordinate(int coord);

signals:
    void pathStep(int shift, int coordinateInPathStep);

public slots:
    void proc1ms();

private:

    int _stepCoordinate;
    int _startStepCoordinate;
    double _coordinate;
    double _lastCoordinate;
    double _targetCoordinate; // координата из сообщения NextTrackCoordinateId

    double _currentV; // текущая скорость, мм/мс
    double _targetV;
    double _lastCoordDiscrepancy; // невязка по координате на прошлом проходе proc1ms()
    double _currentVCorrection; // коррекция текущей скорости

    int _timeCorrection; // поправка времени, если здесь время опережает

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
    void setCoordinate(double coord);
};


#endif // TROLLEY_H




//

