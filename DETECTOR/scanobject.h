#ifndef SCANOBJECT_H
#define SCANOBJECT_H

#include <QVector>
#include "signalsdata.h"


class SCANOBJECT
{
private:

QVector<SignalsData> _data;
public:
    SCANOBJECT();
    ~SCANOBJECT();

// ��������� ������ � ����� �������
    void add(SignalsData *pData);
//
// ���� ������ ��� offset �� ����������, �� ���������� ������ ��������� � �������� ���������
// ������ � �������. ���� ������� ���, �� � offset �������� 0, �.�. ������ �������
// (���������) ��������� ������������ ������
//
    SignalsData *data(unsigned int offset);
    void setPathStep(unsigned int step); // ������ �������� �� (� ����� ����� ��) ��� ����� �������
    unsigned int getPathStep();

    void clear();
    bool isEmpty();
    unsigned int len(); // ���������� ����� ������� � �����������
    unsigned int size();// ���������� ����� ������� � ����� �� - ������ ������� ������
private:
    unsigned int _step; // ��� �� ��� ����� ������� � ����� ����� ��
};


#endif // SCANOBJECT_H
