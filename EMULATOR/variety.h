#ifndef VARIETY_H
#define VARIETY_H

// �������� ������� ����� �� ������ ����� � ��
unsigned int getCurrentTime();
//
// �������� ��������� �������� � �� ����� startOfInterval � ������� ��������
// startOfInterval ���������� � ������� getCurrentTime()
unsigned int getTimeInterval(unsigned int startOfInterval, unsigned int *pCurrentTime);

// ���������� ��������� ����� ����� ���������� min � max
// ��������������, ��� ������� srand() ��� ��������
unsigned char getRandomNumber(int min, int max);

#endif // VARIETY_H
