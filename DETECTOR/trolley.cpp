#include <assert.h>
#include <QDebug>
#include <math.h>
#include "trolley.h"


//
const float TROLLEY::step = 1.83;
const unsigned int TROLLEY::_timerPeriod = 1;
const double TROLLEY::_absDiscrepancyMaxOfCoord = 2.0; // ������������ ������� ����������
const int TROLLEY::_TimeLag = 10;


TROLLEY::TROLLEY(cCriticalSection *cs): _timeCorrection(0),
                    _currentV(0.0)
{
    _cs = cs;
    _rotationDegree = 0.0;
//    setCoordinate(950000.0);
     setCoordinate(0.0, 0.0, 0.0);

    _lastCoordDiscrepancy = 0.0;
    _currentVCorrection = 0.0;
    _targetV = 0.0;
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

// coord, coordL, coordR, time  ������ ���������� ��������� � ���������� � �������� ������ �������
//
void TROLLEY::changeMovingParameters(float targetSpeed, int coord, int coordL, int coordR, unsigned int time)
{
tMovingTarget target;
    _cs->Enter();
    isTagetTimeCorrect(time);
//    if (isTargetTimeCorrect() == false) return;

    target.Time = time;
    target.StartCoord = coord;
    target.StartCoordL = coordL;
    target.StartCoordR = coordR;
    target.TargetSpeed = (double)targetSpeed / 1000.0; // ��/� -> ��/��
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
    qDebug() << "�������� �����������";
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

void TROLLEY::proc1ms()
{
float period = 1.0; // ��
unsigned int currentms =  getCurrentTime(true);
double coordDiscrepancy;
int timeDiscrepancy;

    _cs->Enter();

// ����������� �������� �� ��������� ������ ����� ��������
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
    { // ������� ����������
//        qDebug() << "coord discrepancy = " << coordDiscrepancy << "_coordinate =" << _coordinate;
    }

    if (_targetV != _targets.front().TargetSpeed)
    {
        _targetV = _targets.front().TargetSpeed;
        if (_targetV != 0.0) _currentV = _targetV;
            else
            {// ������ ������������

                qDebug()<< "need to stop";

                if ((_currentV * coordDiscrepancy) >= 0)
                { // ���� ���� ��� ��������, ���� ����� � ������ ��������
                    _currentV = 0.0;
                    qDebug() << "Stopped without delay on coordinate" << _coordinate;
                }
            }
        _currentVCorrection = 0.0;
//        qDebug() << "targetV set to " << _targetV;
    }
        else
        {
            if ((fabs(coordDiscrepancy) > _absDiscrepancyMaxOfCoord) && (_targetV != 0.0) )
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
// ������, ����� _currentV �� ������� ����
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

        if (_targetV != 0.0)
        {
            setTrolleyTargetRotation((double)_targets.front().StartCoordL, (double)_targets.front().StartCoordR);
        }
   } //
    _targets.pop_front();
}
    if ((_targetV == 0.0) && (_currentV != 0.0))
    {  // coordDiscrepancy � _currentV ����� ���� ������ ������
       // ����� �� _currentV ���� ����� ����������� � ����
        coordDiscrepancy = _coordinate - _targetCoordinate;
        if (fabs(coordDiscrepancy) > fabs(_currentV))
        {
            _coordinate += _currentV;
//            qDebug() << "stopping: _coordinate changed to " << _coordinate << "by _currentV = " << _currentV;
        }
            else
            {
               _coordinate -= coordDiscrepancy;
               _currentV = 0.0;
//               qDebug() << "stopping: _coordinate changed to " << _coordinate << "by coordDiscrepancy = " << coordDiscrepancy;
            }
    }
        else _coordinate += _currentV; // �� �������� 1 ��
//
    if (_coordinate < _targetCoordinate)
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

// �������� ������� ����� �� ������ ����� � ������ ��������
unsigned int TROLLEY::getCurrentTime(bool withCorrection)
{
unsigned int res;
QTime currentT = QTime::currentTime();
    if (withCorrection) currentT = currentT.addMSecs(_timeCorrection);
    res = currentT.msecsSinceStartOfDay();
    return res;
}

// timeByMS ������ ���� �� ������ �������� ������� � ������ ���������
bool TROLLEY::isTagetTimeCorrect(unsigned int timeByMS)
{
unsigned int correctedTime = getCurrentTime(true);
int deltaT = timeByMS - correctedTime;
bool res = true;

    if ((deltaT < 0) || (deltaT > _TimeLag))
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
                _timeCorrection =  deltaT - _TimeLag; // ����� ������� ������ ������� timeByMS ������� � ��������� �� 10 ��
//                qDebug() << "TROLLEY::isTagetTimeCorrect: �������� �������� ������� �� "<< _timeCorrection << "��";
            }
        }
            else if (deltaT > _TimeLag)
                 {
                     _timeCorrection = deltaT - _TimeLag;
//                     qDebug() << "TROLLEY::isTagetTimeCorrect: �������� �������� ������� �� "<< _timeCorrection << "��";
                 }
    }
    return res;
}
