#ifndef DEFSUBST_H
#define DEFSUBST_H

#define TimeToTick(time) time

#define __irq
#define __arm

#define __no_init
#define TBD
#define PARAM_UNDEFINED

typedef class cCriticalSection* xSemaphoreHandle;
#define xTaskHandle int

#define smprintf simplePrintf
#define simplePrintf(c, ...) // qWarning()<<c

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef unsigned char UCHAR;
typedef unsigned short USHORT;


#endif // DEFSUBST_H
