
#ifndef _us465dh
#define _us465dh


extern xSemaphoreHandle s_ImitDP;   

#ifndef AC_dis
extern tACThLine *pAcThLine0;
extern tACThLine *pAcThLine1;
#endif

extern volatile int lDiff;

#ifdef __cplusplus

void ush_init(void);
void extMemBusInit(void); 
void h_init(void);
void hDeinit(void);

unsigned char lanmsgparcer(UCHAR* buf, USHORT lng);
//
void ustsk(void *ppar);

void ASD_On(void);
void ASD_Off(void);
void ascanSumOn(void);
void acMessageOff(void);
void acStateOn(void);
void BUMctrlproc(UCHAR *ptr);
void make_C3(void);
void SendVersionInfo(void);
void StartAScan(UCHAR *ptr);
void varsinit(void);
void setDPImitCmd(UCHAR *ptr);
void SetDPval(UCHAR *p);
void setDPImitExCmd(UCHAR *p);
void setScanImitCmd(UCHAR *p);
void doWhenImitSwitchOFF(unsigned char num);
void scanerImitProc(void);
void sendDPData(unsigned char sensorNum, unsigned char sensorData);
void setDPOrImitValue(unsigned char *p,unsigned char fExtendedCmd);
void setImit(unsigned char *p,unsigned char fExtendedCmd);
inline unsigned char getDPShift(UCHAR relativeNumber);
inline unsigned char getImitShift(UCHAR relativeNumber);

void redefineDPCycleProc(void);

void Start_PLD_Work(void);
void Stop_PLD_Work(void);
void ReadASD(void);
unsigned char getShiftSensorValue(UCHAR number);

//void commutatePortsAndPullResistors(unsigned char where);
void pinToPullResCommutate(unsigned char port, unsigned char pinInPort, unsigned char where);

void initMScanFlags(void);

#else

#define ush_init _Z7ush_initv
#define extMemBusInit _Z13extMemBusInitv
#define h_init _Z6h_initv 
#define hDeinit _Z7hDeinitv
#define lanmsgparcer _Z12lanmsgparcerPht
#define ustsk _Z5ustskPv
#define commutatePortsAndPullResistors _Z30commutatePortsAndPullResistorsh
#define pinToPullResCommutate _Z21pinToPullResCommutatehhh

void _Z6h_initv(void);
void _Z13extMemBusInitv(void);
void _Z7hDeinitv(void);
void _Z7ush_initv(void);
void _Z12lanmsgparcerPht(UCHAR* buf, USHORT lng);
void _Z5ustskPv(void *ppar);
void _Z30commutatePortsAndPullResistorsh(unsigned char where);
void _Z21pinToPullResCommutatehhh(unsigned char port, unsigned char pinInPort, unsigned char where);

#endif

#define COND_IntASD_ENABLE \
{\
    if (fIntPLDMustBeEnable) enintpld;\
}

//при вызове должно быть исключено переключение задач
#define COND_INT_ASD_ENABLE_LOCK \
{\
    fIntPLDMustBeEnable = 0;\
}
//
//при вызове должно быть исключено переключение задач
#define COND_INT_ASD_ENABLE_UNLOCK \
{\
    fIntPLDMustBeEnable = 1;\
}

//
#define DP_SEMAPHORE_ENTER \
{\
disintpld \
TakeSemaphore(s_ImitDP);\
}

#define DP_SEMAPHORE_LEAVE \
{\
xSemaphoreGive(s_ImitDP);\
COND_IntASD_ENABLE \
}

#define REDEFINE_DP_CYCLE_PROC \
{\
DP_SEMAPHORE_ENTER \
redefineDPCycleProc(); \
DP_SEMAPHORE_LEAVE\
}

#endif
