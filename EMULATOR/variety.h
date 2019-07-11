#ifndef VARIETY_H
#define VARIETY_H

// получить текущее время от начала суток в мс
unsigned int getCurrentTime();
//
// получить временной интервал в мс между startOfInterval и текущим моментом
// startOfInterval определяем с помощью getCurrentTime()
unsigned int getTimeInterval(unsigned int startOfInterval, unsigned int *pCurrentTime);

// Генерируем рандомное число между значениями min и max
// Предполагается, что функцию srand() уже вызывали
unsigned char getRandomNumber(int min, int max);

#endif // VARIETY_H
