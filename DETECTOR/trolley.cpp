#include <assert.h>
#include <QDebug>
#include <math.h>
#include "trolley.h"
#include "defsubst.h"
#include "us465d.h"
#include "ustyp468.h"
#include "variety.h"

//
const double TROLLEY::step = 1.83;
const unsigned int TROLLEY::_timerPeriod = 1;
const double TROLLEY::_absDiscrepancyMaxOfCoord = 3.0; // максимальная невязка координаты

TROLLEY::TROLLEY(cCriticalSection *cs): _currentV(0.0),
                                        _rotationDegree(0.0),
                                        _extrapolationTime(60.0)
{
    _cs = cs;
    _correctionCounterInit = floor(_extrapolationTime / TROLLEY::_timerPeriod);
//    setCoordinate(950000.0);
     setCoordinate(0.0, 0.0, 0.0);

//    _lastCoordDiscrepancy = 0.0;
    _targetV = 0.0;
    setMovingDirection(Test::DirUpWard);

    _timeSinceMidNight = getCurrentTime();
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
#ifdef FAKE_V_CALCULATION
    lDiff = (int)fabs(target.TargetSpeed * cSpeedCalcTime / step);
    if (lDiff > 0xFFFF) lDiff > 0xFFFF;
#endif
    _targets.push_back(target);

//    if (abs(coordL - coordR) > 10) qWarning() << "MovPar:" << "coordL =" << coordL << "coordR =" << coordR << "diff =" << coordL - coordR;

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
//    qWarning() << "движение остановлено";
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
//        qWarning() << "msec = " << curT.msec() << "step = " << c - _stepCoordinate << " path = " << _coordinate << "mm";

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

    if (_targetCoordinate != _coordinate)
    {
        _rotationCoefficient = (_targetRotationDegree - _rotationDegree) / (_targetCoordinate - _coordinate);
    }
        else _rotationCoefficient = 0.0;
    _memCoordinate = _coordinate;
    _memRotationDegree = _rotationDegree;
}

// расчет скорости мм/мс, которая требуется, чтобы переместиться из currentCoordinate в targetCoordinate за timePeriod
// координаты задабются в мм, время - в мс
double TROLLEY::VCalculate(double currentCoordinate, double targetCoordinate, double timePeriod)
{
double res = (targetCoordinate - currentCoordinate)/timePeriod;

//    qWarning() << "VCalculate: coordinate =" << currentCoordinate << "targetCoordinate =" << targetCoordinate << "res =" << res;
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
unsigned int currentTime;
unsigned int timeSpan;

    _cs->Enter();
    timeSpan = getTimeInterval(_timeSinceMidNight, &currentTime);
    _timeSinceMidNight = currentTime;
//
    skipVCorrection = false;
    if(!_targets.isEmpty())
    {
       skipVCorrection = true;
       _targetCoordinate = _targets.back().StartCoord * 1.0;
       if (_targets.back().TargetSpeed == 0.0)
       {
           if (_targetV != 0.0)
           {
               setTrolleyTargetRotation((double)_targets.back().StartCoordL, (double)_targets.back().StartCoordR);
               changeCoordinate(timeSpan);
               coordDiscrepancy = _coordinate - _targetCoordinate;
               _targetV = 0.0;
               if (((_currentV * coordDiscrepancy) > 0.0) || (coordDiscrepancy == 0.0) )
               { // если либо уже добежали, либо бежим в переди паровоза
                   _currentV = 0.0;
//                    qWarning() << "Stopped without delay on coordinate" << _coordinate << "_coordL =" << _coordinate + _rotationDegree * 0.5 << "_coordR =" << _coordinate - _rotationDegree * 0.5;
                   _movingState = Normal;
               }
                   else
                   {// определение скорости торможения, чтобы остановиться за 2.0 мС
                   double v = VCalculate(_coordinate, _targetCoordinate, 2.0);
                       if (fabs(_currentV) < fabs(v))
                       {
                           _currentV = v;
                       }
                       _stoppingStartTime = getCurrentTime();
                       _movingState = Stopping;
                   }
          }
              else skipVCorrection = false;
       }
           else
           {
                setTrolleyTargetRotation((double)_targets.back().StartCoordL, (double)_targets.back().StartCoordR);
                changeCoordinate(timeSpan);
                coordDiscrepancy = _coordinate - _targetCoordinate;
                _targetV = _targets.back().TargetSpeed;
                if (fabs(coordDiscrepancy) > _absDiscrepancyMaxOfCoord)
                {
                    bool abruptDiminution = false;
                    double extapolatedCoordinate = _targetCoordinate + _targetV * _extrapolationTime;
                    if (_targetV >= 0)
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
//                        qWarning() << "V corrected to" << _currentV << "_coordinate =" << _coordinate << "extapolatedCoordinate =" << extapolatedCoordinate << "_targetCoordinate =" << _targetCoordinate;
                    }
                        else
                        {
                            _currentV = _currentV / 4.0;
//                            qWarning() << "V decreaced to" << _currentV;
                        }

                    _correctionCounter = _correctionCounterInit;
                    _movingState = Correction;
                }
                   else
                   {
                       _currentV = _targetV;
                       _movingState = Normal;
//                       qWarning() << "V set to" << _currentV;
                   }
            }
       _targets.clear();
    }
//
    if (!skipVCorrection)
    {
            switch(_movingState)
            {
                case Stopping:
                {
                unsigned int stoppingDuration = getTimeInterval(_stoppingStartTime, nullptr);
                    coordDiscrepancy = _coordinate - _targetCoordinate;
                    if (fabs(coordDiscrepancy) > fabs(_currentV))
                    {
                        changeCoordinate(1); // изменяем координату только на _currentV, не бошльше - см условие if
                        if (stoppingDuration >= queerStoppingTime)
                        {
//                            qWarning() << "stopped: _coordinate changed to " << _coordinate << "by currentV = " << _currentV << "_coordL =" << _coordinate + _rotationDegree * 0.5 << "_coordR =" << _coordinate - _rotationDegree * 0.5 << "targetCoord =" << _targetCoordinate;
                        }
                    }
                        else
                        {
                            _coordinate -= coordDiscrepancy;
                            _currentV = 0.0;
                            _movingState = Normal;
                            if (stoppingDuration >= queerStoppingTime)
                            {
//                                qWarning() << "stopped: _coordinate changed to " << _coordinate << "by coordDiscrepancy = " << coordDiscrepancy << "_coordL =" << _coordinate + _rotationDegree * 0.5 << "_coordR =" << _coordinate - _rotationDegree * 0.5;
                            }
                        }
                break;
                }
                case Correction:
                    changeCoordinate(timeSpan);
                    if (_correctionCounter)
                    {
                        _correctionCounter--;
                    }
                    if (_correctionCounter == 0)
                    {
                        _currentV = _targetV;
                        _movingState = Normal;
//                        qWarning() << "V restored to" << _currentV;
                    }
                break;
                default: // Normal
                   changeCoordinate(timeSpan);
                break;
            }
    }

    if (_coordinate != _lastCoordinate)
    {
//        qWarnin() << "Coordinate = " << _coordinate;
        _lastCoordinate = _coordinate;
    }

    _cs->Release();
}

void TROLLEY::setMovingDirection(Test::eMovingDir movingDirection)
{
    _movingDirection = movingDirection;
}
