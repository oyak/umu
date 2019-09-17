#include <math.h>
#include <QTime>

// получить текущее время от начала суток в мс
unsigned int getCurrentTime()
{
 unsigned int res;
 QTime currentT = QTime::currentTime();
     res = currentT.msecsSinceStartOfDay();
     return res;
}

// получить временной интервал в мс между startOfInterval и текущим моментом
// startOfInterval определяем с помощью getCurrentTime()
unsigned int getTimeInterval(unsigned int startOfInterval, unsigned int *pCurrentTime)
{
unsigned int currentTime = getCurrentTime();
unsigned int res;

    if (currentTime >= startOfInterval)
    {
        res = currentTime - startOfInterval;
    }
        else
        {
            res = 86400000 - startOfInterval + currentTime;
        }
    if (pCurrentTime) *pCurrentTime = currentTime;
    return res;
}

// Генерируем случайное число между значениями min и max
// Предполагается, что функцию srand() уже вызывали
unsigned char getRandomNumber(int min, int max)
{
const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    // равномерно распределяем число в нашем диапазоне
    return static_cast<unsigned char>(qrand() * fraction * (max - min + 1) + min);

}
