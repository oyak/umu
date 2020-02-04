#ifndef DEFSUBST_H
#define DEFSUBST_H

#ifndef pconsth46
//#error file pconst.h must be included before this one
#endif

#define TimeToTick(time) time

#define TBD
#define PARAM_UNDEFINED

typedef class cCriticalSection* xSemaphoreHandle;
#define xTaskHandle int

#define smprintf simplePrintf
#define simplePrintf(c, ...) while(0) // qWarning()<<c

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef unsigned char UCHAR;
typedef unsigned short USHORT;

#define idBUM 4
// fields offset() respectively start of message's body
#define slt_bidx 0

#ifdef OLD_PACKET_HEADER
// fields offset respectively start of message
#define slt_offs  4
#define asbdy_offs 5 // a-razv block
#else
#define slt_offs  6
#define asbdy_offs 7 // a-razv block

#endif

#define tempbuf_size 512
#define o1data_size 16

#define TprizmMax 250
#define TprizmLargeMax 1500

#define QDP   4  // ammount of DP
#define IMITATORMASK 0x4

#define AscanSendTime 50  // ms
#define ASDSendTime 50
#define MScanSendTime 50  // mS - if it will be reached, set fMScan and send MScan
#define minImitDPIncTime 20   // минимальное значение imitDPIncTime[] в м—

#define Set_CSPLD
#define Reset_CSPLD

#define RESET_INT_PLD

#define PLD_INT_PIN_STATE

#define disintT1

#define RESET_INT_T1

#define INT_T1_PENDING
#define enintT1

#define START_BSCAN_PERIOD
#define BSCAN_READY_SIGN_CLEAR


#endif // DEFSUBST_H
