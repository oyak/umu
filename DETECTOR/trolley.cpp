#include <assert.h>
#include <QDebug>
#include <math.h>
#include "trolley.h"


//
const float TROLLEY::step = 1.83;
const unsigned int TROLLEY::_timerPeriod = 1;
const double TROLLEY::_absDiscrepancyMaxOfCoord = 2.0; // максимальная невязка координаты

TROLLEY::TROLLEY(cCriticalSection *cs): _currentV(0.0),
                                        _rotationDegree(0.0),
                                        _extrapolationTime(12.0)
{
    _cs = cs;
    _correctionCounterInit = floor(_extrapolationTime / TROLLEY::_timerPeriod);
//    setCoordinate(950000.0);
     setCoordinate(0.0, 0.0, 0.0);

//    _lastCoordDiscrepancy = 0.0;
    _targetV = 0.0;
    setMovingDirection(Test::DirUpWard);
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

// coord, coordL, coordR, time  задают требование оказаться в координате в заданный момент времени
//
void TROLLEY::changeMovingParameters(float targetSpeed, int coord, int coordL, int coordR, unsigned int time)
{
tMovingTarget target;
    _cs->Enter();

    target.Time = time;
    target.StartCoord = coord;
    target.StartCoordL = coordL;
    target.StartCoordR = coordR;
    target.TargetSpeed = (double)targetSpeed / 1000.0; // мм/с -> мм/мс
    _targets.push_back(target);

//    if (abs(coordL - coordR) > 10) qDebug() << "MovPar:" << "coordL =" << coordL << "coordR =" << coordR << "diff =" << coordL - coordR;

    _cs->Release();
}

void TROLLEY::setCoordinate(double coord, double coordL, double coordR)
{
    _lastCoordinate = _coordinate = _targetCoordinate = coord;
    _startStepCoordinate = _coordinate / step;
    _stepCoordinate = _startStepCoordinate;
    setTrolleyTargetRotation(coordL, coordR);
}
void TROLLEY::setCoordinate(int coord, int coordL, int coordR)
{
    _cs->Enter();
    setCoordinate((double)coord, (double)coordL, (double)coordR);
    _cs->Release();
}
void TROLLEY::stopTrolley()
{
    _targetV = 0.0;
    stopMoving();
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
//    QTime curT;
        double shift = _rotationDegree * 0.5;

        emit pathStep(c - _stepCoordinate, (int)(_coordinate + shift), (int)(_coordinate - shift));

//        curT = QTime::currentTime();
//        qDebug() << "msec = " << curT.msec() << "step = " << c - _stepCoordinate << " path = " << _coordinate << "mm";

        _stepCoordinate = c;
    }
}

float TROLLEY::getDisplacement(float v0, float a, unsigned int time)
{
    return (v0 + a * time) * time;
}

void TROLLEY::setTrolleyTargetRotation(double targetCoordL, double targetCoordR)
{
    _targetRotationDegree = targetCoordL - targetCoordR;

    if (_targetCoordinate != _coordinate) _rotationCoefficient = (_targetRotationDegree - _rotationDegree) / (_targetCoordinate - _coordinate);
        else _rotationCoefficient = 0.0;
    _memCoordinate = _coordinate;
    _memRotationDegree = _rotationDegree;
}

// расчет скорости мм/мс, которая требуется, чтобы переместиться из currentCoordinate в targetCoordinate за timePeriod
// координаты задабются в мм, время - в мс
double TROLLEY::VCalculate(double currentCoordinate, double targetCoordinate, double timePeriod)
{
double res = (targetCoordinate - currentCoordinate)/timePeriod;
    if (fabs(res) > AbsMaxV)
    {
        if (res > 0.0)
        {
            res = AbsMaxV;
        }
            else
            {
                res = AbsMaxV * (-1);
            }
    }
    return res;
}

void TROLLEY::proc1ms()
{
double coordDiscrepancy;
bool skipVCorrection;

    _cs->Enter();
    skipVCorrection = false;
    if(!_targets.isEmpty())
    {
       skipVCorrection = true;
       _targetCoordinate = _targets.back().StartCoord * 1.0;
       coordDiscrepancy = _coordinate - _targetCoordinate;
       if (_targets.back().TargetSpeed == 0.0)
       {
           if (_targetV != 0.0)
           {
               _coordinate += _currentV; // за интервал 1 мС
               _targetV = 0.0;
               if ((_currentV * coordDiscrepancy) >= 0.0)
               { // если либо уже добежали, либо бежим в переди паровоза
               _currentV = 0.0;
                    qDebug() << "Stopped without delay on coordinate" << _coordinate << "_coordL =" << _coordinate + _rotationDegree * 0.5 << "_coordR =" << _coordinate - _rotationDegree * 0.5;
               _movingState = Normal;
               }
                   else
                   {// пересчет текущей скорости, если она меньше текущей
                   double v = VCalculate(_coordinate, _targetCoordinate, 2.0);
                       if (_currentV < v)
                       {
                           _currentV = v;
                       }
                       _movingState = Stopping;
                   }
          }
              else skipVCorrection = false;
       }
           else
           {
                _targetV = _targets.back().TargetSpeed;
                _coordinate += _currentV; // за интервал 1 мС
                if (fabs(coordDiscrepancy) > _absDiscrepancyMaxOfCoord)
                {
                    bool abruptDiminution = false;
                    double extapolatedCoordinate = _targetCoordinate + _targetV * _extrapolationTime;
                    if (_movingDirection == Test::DirUpWard)
                    {
                        if (extapolatedCoordinate <=  _coordinate)
                        { // требуется резкое снижение скорости
                            abruptDiminution = true;
                        }
                    }
                        else
                        {
                            if (extapolatedCoordinate >=  _coordinate)
                            {
                                abruptDiminution = true;
                            }
                        }

                    if (!abruptDiminution)
                    {
                        _currentV = VCalculate(_coordinate, extapolatedCoordinate, _extrapolationTime);
                        qDebug() << "V corrected to" << _currentV;

                    }
                        else
                        {
                            _currentV = _currentV / 4.0;
                            qDebug() << "V decreaced to" << _currentV;
                        }

                    _correctionCounter = _correctionCounterInit;
                    _movingState = Correction;
                }
                   else
                   {
                       _movingState = Normal;
                   }
            }
       if (_targetV != 0.0)
       {
           setTrolleyTargetRotation((double)_targets.back().StartCoordL, (double)_targets.back().StartCoordR);
       }
       _targets.clear();
    }
//
    if (!skipVCorrection)
    {
            switch(_movingState)
            {
                case Stopping:
                coordDiscrepancy = _coordinate - _targetCoordinate;
                if (fabs(coordDiscrepancy) > fabs(_currentV))
                {
                    _coordinate += _currentV;
                    qDebug() << "stopping: _coordinate changed to " << _coordinate << "by _currentV = " << _currentV;
                }
                    else
                    {
                       _coordinate -= coordDiscrepancy;
                       _currentV = 0.0;
                       _movingState = Normal;
                       qDebug() << "stopped: _coordinate changed to " << _coordinate << "by coordDiscrepancy = " << coordDiscrepancy << "_coordL =" << _coordinate + _rotationDegree * 0.5 << "_coordR =" << _coordinate - _rotationDegree * 0.5;
                    }
                break;
                case Correction:
                    _coordinate += _currentV; // за интервал 1 мС
                    if (_correctionCounter)
                    {
                        _correctionCounter--;
                    }
                    if (_correctionCounter == 0)
                    {
                        _currentV = _targetV;
                        _movingState = Normal;
                        qDebug() << "V restored to" << _currentV;
                    }
                break;
                default: // Normal
                    _coordinate += _currentV; // за интервал 1 мС
                break;
            }
    }

    if ((_movingDirection == Test::DirUpWard) && (_coordinate < _targetCoordinate) || (_movingDirection == Test::DirDownWard) && (_coordinate > _targetCoordinate))
    {
        _rotationDegree = _rotationCoefficient * (_coordinate - _memCoordinate) + _memRotationDegree;
    }
        else _rotationDegree = _targetRotationDegree;
    //
    if (_coordinate != _lastCoordinate)
    {
//        qDebug() << "Coordinate = " << _coordinate;
        _lastCoordinate = _coordinate;
    }

    ifPathEvent();

    _cs->Release();
}

void TROLLEY::setMovingDirection(Test::eMovingDir movingDirection)
{
    _movingDirection = movingDirection;
}


// получить текущее время от начала суток с учетом поправки
unsigned int TROLLEY::getCurrentTime(bool withCorrection)
{
unsigned int res;
QTime currentT = QTime::currentTime();
//    if (withCorrection) currentT = currentT.addMSecs(_timeCorrection);
    res = currentT.msecsSinceStartOfDay();
    return res;
}

