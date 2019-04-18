#include <assert.h>
#include <QDebug>
#include <math.h>
#include "trolley.h"


//
const float TROLLEY::step = 1.83;
const unsigned int TROLLEY::_timerPeriod = 1;
const double TROLLEY::_absDiscrepancyMaxOfCoord = 2.0; // максимальная невязка координаты
const int TROLLEY::_TimeLug = 10;


TROLLEY::TROLLEY(cCriticalSection *cs): _timeCorrection(0),
                    _currentV(0.0)
{
//    setCoordinate(950000.0);
     setCoordinate(0.0);

    _lastCoordDiscrepancy = 0.0;
    _currentVCorrection = 0.0;
    _targetV = 0.0;
    _cs = cs;
    _trolleyTimer1ms.setInterval(1);
    _trolleyTimer1ms.start();
    connect(&_trolleyTimer1ms, SIGNAL(timeout()), this, SLOT(proc1ms()));
}

TROLLEY::~TROLLEY()
{
    _trolleyTimer1ms.stop();
     disconnect(&_trolleyTimer1ms, SIGNAL(timeout()), this, SLOT(proc1ms()));
    _targets.clear();

}

// coord, time  задают требование оказаться в координате в заданный момент времени
//
void TROLLEY::changeMovingParameters(float targetSpeed, int coord, unsigned int time)
{
tMovingTarget target;
    _cs->Enter();
    isTagetTimeCorrect(time);
//    if (isTargetTimeCorrect() == false) return;

    target.Time = time;
    target.StartCoord = coord;
    target.TargetSpeed = (double)targetSpeed / 1000.0; // мм/с -> мм/мс
    _targets.push_back(target);
    _cs->Release();
}

void TROLLEY::setCoordinate(double coord)
{
    _lastCoordinate = _coordinate = _targetCoordinate = coord;
    _startStepCoordinate = _coordinate / step;
    _stepCoordinate = _startStepCoordinate;

}
void TROLLEY::setCoordinate(int coord)
{
    _cs->Enter();
    setCoordinate((double)coord);
    _cs->Release();
}
void TROLLEY::stopMoving()
{
    _currentV = 0;
    ifPathEvent();
    qDebug() << "движение остановлено";
}

void TROLLEY::ifPathEvent()
{
int c = _coordinate / step;
    if (c != _stepCoordinate)
    {
    QTime curT;
        emit pathStep(c - _stepCoordinate, (int)_coordinate);

        curT = QTime::currentTime();
//        qDebug() << "msec = " << curT.msec() << "step = " << c - _stepCoordinate << " path = " << _coordinate << "mm";

        _stepCoordinate = c;

    }
}

float TROLLEY::getDisplacement(float v0, float a, unsigned int time)
{
    return (v0 + a * time) * time;
}

void TROLLEY::proc1ms()
{
float period = 1.0; // мс
unsigned int currentms =  getCurrentTime(true);
double coordDiscrepancy;
int timeDiscrepancy;

    _cs->Enter();

// определение смещения за прошедший период между вызовами
//double deltaS = _currentV; // * period;

if (!_targets.isEmpty() && (_targets.front().Time <= currentms))
{
    timeDiscrepancy = currentms - _targets.front().Time;
    if ( timeDiscrepancy > 1)
    {
//        qDebug() << "target time discrepancy = " << currentms - _targets.front().Time;
    }
   if ((timeDiscrepancy < 10) || (_targets.size() == 1))
   { //
    _targetCoordinate = _targets.front().StartCoord * 1.0;
    coordDiscrepancy = _coordinate - _targetCoordinate;

    if (fabs(coordDiscrepancy) > _absDiscrepancyMaxOfCoord)
    { // невязка координаты
//        qDebug() << "coord discrepancy = " << coordDiscrepancy << "_coordinate =" << _coordinate;
    }

    if (_targetV != _targets.front().TargetSpeed)
    {
        _targetV = _targets.front().TargetSpeed;
        if (_targetV != 0.0) _currentV = _targetV;
            else
            {// должны остановиться
                if ((_currentV * coordDiscrepancy) >= 0)
                { // если либо уже добежали, либо бежим в переди паровоза
                    _currentV = 0.0;
                }
            }
        _currentVCorrection = 0.0;
//        qDebug() << "targetV set to " << _targetV;
    }
        else
        {
            if (fabs(coordDiscrepancy) > _absDiscrepancyMaxOfCoord)
            {
                if ((fabs(coordDiscrepancy) >= abs(_lastCoordDiscrepancy)) || ((coordDiscrepancy * _lastCoordDiscrepancy) < 0))
                {
//                    qDebug() << "prepare to coorect _currentV =" << _currentV;
                    _currentVCorrection = (coordDiscrepancy - _lastCoordDiscrepancy) / -10.0;
                    if (fabs(_currentVCorrection) > fabs(_targetV) * 0.2)
                    {
                        if (_currentVCorrection > 0) _currentVCorrection = fabs(_targetV) * 0.2;
                            else _currentVCorrection = fabs(_targetV) * (-0.2);
                    }
// следим, чтобы _currentV не сменило знак
                    if (_currentV * _currentVCorrection < 0)
                    {
                        if (fabs(_currentVCorrection) > fabs(_currentV))
                            _currentV /= 2.0;
                    }
                        else
                        {
                            if (fabs(_currentVCorrection) == fabs(_currentV))
                            {
                                _currentV /= 2.0;
                            }
                        }
                    _currentV += _currentVCorrection;
//                    qDebug() << "V corrected to " << _currentV << "by correction = " << _currentVCorrection;
                }
            }
            _lastCoordDiscrepancy = coordDiscrepancy;
        }
   } //
    _targets.pop_front();
}
    if ((_targetV == 0.0) && (_currentV != 0.0))
    {  // coordDiscrepancy и _currentV ранее были разных знаков
       // иначе бы _currentV была сразу установлена в ноль
        coordDiscrepancy = _coordinate - _targetCoordinate;
        if (fabs(coordDiscrepancy) > fabs(_currentV))
        {
            _coordinate += _currentV;
//            qDebug() << "stopping: _coordinate changed to " << _coordinate << "by _currentV = " << _currentV;
        }
            else
            {
               _coordinate -= coordDiscrepancy;
//               qDebug() << "stopping: _coordinate changed to " << _coordinate << "by coordDiscrepancy = " << coordDiscrepancy;
            }
    }
        else _coordinate += _currentV; // за интервал 1 мС

    if (_coordinate != _lastCoordinate)
    {
//        qDebug() << "Coordinate = " << _coordinate;
        _lastCoordinate = _coordinate;
    }

    ifPathEvent();

    _cs->Release();
}

// получить текущее время от начала суток с учетом поправки
unsigned int TROLLEY::getCurrentTime(bool withCorrection)
{
unsigned int res;
QTime currentT = QTime::currentTime();
    if (withCorrection) currentT = currentT.addMSecs(_timeCorrection);
    res = currentT.msecsSinceStartOfDay();
    return res;
}

// timeByMS должно быть не меньше текущего времени с учетом коррекции
bool TROLLEY::isTagetTimeCorrect(unsigned int timeByMS)
{
unsigned int correctedTime = getCurrentTime(true);
int deltaT = timeByMS - correctedTime;
bool res = true;

    if ((deltaT < 0) || (deltaT > _TimeLug))
    {
        correctedTime = getCurrentTime(false);
        deltaT = timeByMS - correctedTime;
        res = false;
        if (deltaT < 0)
        {
            QTime targetT;
            targetT.fromMSecsSinceStartOfDay(timeByMS);
            if (targetT.hour() != 0)
            {
                _timeCorrection =  deltaT - _TimeLug; // чтобы сделать момент времени timeByMS будущим и отстоящим на 10 мС
//                qDebug() << "TROLLEY::isTagetTimeCorrect: изменена поправка времени до "<< _timeCorrection << "мС";
            }
        }
            else if (deltaT > _TimeLug)
                 {
                     _timeCorrection = deltaT - _TimeLug;
//                     qDebug() << "TROLLEY::isTagetTimeCorrect: изменена поправка времени до "<< _timeCorrection << "мС";
                 }
    }
    return res;
}



