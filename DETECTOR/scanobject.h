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

// добавляет данные в конец массива
    void add(SignalsData *pData);
//
// если данных для offset не существует, то возвращаем пустой указатель и смещение следующих
// данных в векторе. Если таковых нет, то в offset помещаем 0, т.к. данные нулевым
// (начальным) смещением присутствуют всегда
//
    SignalsData *data(unsigned int offset);
    void setPathStep(unsigned int step); // задает значение ДП (в сотых долях мм) для этого объекта
    unsigned int getPathStep();

    void clear();
    bool isEmpty();
    unsigned int len(); // возвращает длину объекта в миллиметрах
    unsigned int size();// возвращает длину объекта в шагах ДП - размер массива данных
private:
    unsigned int _step; // шаг ДП для этого объекта в сотых долях мм
};


#endif // SCANOBJECT_H
