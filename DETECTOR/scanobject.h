#ifndef SCANOBJECT_H
#define SCANOBJECT_H

#include <QVector>
#include <QObject>
#include "signalsdata.h"


class SCANOBJECT:public QObject
{
   Q_OBJECT
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
    void getAllChannels(QVector<CID>& channels);
    void view();

signals:
    void message(QString);

private:
    QVector<SignalsData> _data;
    unsigned int _step; // ��� �� ��� ����� ������� � ����� ����� ��
};


#endif // SCANOBJECT_H
