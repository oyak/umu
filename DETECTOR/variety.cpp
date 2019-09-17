#include <math.h>
#include <QTime>

// �������� ������� ����� �� ������ ����� � ��
unsigned int getCurrentTime()
{
 unsigned int res;
 QTime currentT = QTime::currentTime();
     res = currentT.msecsSinceStartOfDay();
     return res;
}

// �������� ��������� �������� � �� ����� startOfInterval � ������� ��������
// startOfInterval ���������� � ������� getCurrentTime()
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

// ���������� ��������� ����� ����� ���������� min � max
// ��������������, ��� ������� srand() ��� ��������
unsigned char getRandomNumber(int min, int max)
{
const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    // ���������� ������������ ����� � ����� ���������
    return static_cast<unsigned char>(qrand() * fraction * (max - min + 1) + min);

}
