// include file
// ��� ������������� � ������� socket8 
// �������� _us320443.cpp

//  _us320442.cpp
// ush_init() - ��������� ������������� ���������� �� ����� intPLD 
// _us320442.cpp
// ����� ���� ���������� �� ush_init()  � h_init()
// _us320441.cpp
//
//  ��������� �������� ���������� �� LARGE_BSCAN -> TASK_CREATE
// ��������� �������� ���������� �� DP1_HARDWARE � CONFIGURAITON_PRINT

//
// �����:
// ushinit() ������������� � ush_init()
// ������� ��������   h_init();
//
void pinToPullResCommutate(unsigned char port, unsigned char pinInPort, unsigned char where);

#define SETPINMODE(modeRegister)\
{\
register DWORD reg;\
 reg = modeRegister;\
 reg &= ~pinMask;\
 reg |= mask;\
modeRegister = reg;\
}


void varsinit(void);
//
#ifndef DEVICE_EMULATION

void extMemBusInit(void) 
{
int ii;

  PCONP |= (1<<11);         // external mem controller

    PINSEL6 = 0x55555555; // D0- D15
//    PINMODE6 = 0xAAAAAAAA; // neither pull-up nor pull-down
//    PINMODE6 = 0xFFFFFFFF; // pull-down
  
     for(ii=0; ii<=15; ii++)   
     {
        pinToPullResCommutate(3,ii,255); // ������� �������������� � ���������� ���� ������
     }
//
     for(ii=0; ii<=16; ii++)   
     {
        pinToPullResCommutate(4,ii,255); // ������� �������������� � ���������� ���� ������
     }
//
    PINSEL8 = 0x55555555; // A0-A15
    PINSEL9 = 0x15555 | (1<<20); // A23-A16,OE, BLS0
//
    EMCCONTROL = 1; // clear mirror bit
// ����� ������� � ��� 12 ��, ���� ������ CCLK = 13 ��
    EMCSTATICWAITWR0 = 2; //3;  //  regvalue +2 ?   1 -> ~25�� // ��� �������� 2  ������ 37,5��, �������� 50 ��
    EMCSTATICWAITRD0 = 3; //4;  //  regvalue +1 ? // ��� �������� 1 �������� ������������ rd 25 ��, ������ 37,5��, �������� 50 ��

    EMCSTATICWAITTURN0 = 0;  
   
//
    EMCSTATICCNFG0 = 1;    // data 16 bit
//

}
//-------------------------------------------------------------------
void h_init(void) 
{
   extMemBusInit();
   Wr_RegPLD(a0x1300,0);
}
//
//-------------------------------------------------------------------------------------------------
// ���������� �� ������������ ���:
// P0.27, P0.28 - �������� ����
// P0.29 - P0.31 - �� ����� ����������
//
// where = 0, ���������� pull-down
// where = 1, ���������� pull-up
// where > 1, ������ ��������������
//
void pinToPullResCommutate(unsigned char port, unsigned char pinInPort, unsigned char where)
{
DWORD mask;
DWORD pinMask;
//
    if (pinInPort > 31) return;
//
    if (pinInPort <=15)
    {
     pinMask = (3<<(pinInPort<<1));
    }  
        else
        {
            pinMask = (3<<((pinInPort-16)<<1));       
        }  
    
    switch (where)
    {
        case 1:
        {
          
            mask = 0;
            break; 
        } 
        case 0:
        { 
            if (pinInPort <= 15) 
            {
                mask = (3<<(pinInPort<<1));
            } 
                else
               {
                   mask = (3<<((pinInPort-16)<<1));
               }
            break;
        }  
        default:
            if (pinInPort <= 15) 
            {
                mask = (2<<(pinInPort<<1));
            } 
                else
                {
                   mask = (2<<((pinInPort-16)<<1));
                }
     }
//
    switch(port)
    {
        case 0:
        {
            if (pinInPort <= 15)
            {
                SETPINMODE(PINMODE0)
            }
                else 
                {
                    if (pinInPort <= 26) 
                    {
                        SETPINMODE(PINMODE1)
                   }
                       else return;
                }
            break;
        }
        case 1:
        {
            if (pinInPort <= 15)
            {
                SETPINMODE(PINMODE2)
            }
                else 
                {
                    SETPINMODE(PINMODE3)
                }
            break;
        }
        case 2:
        {
            if (pinInPort <= 15)
            {
                SETPINMODE(PINMODE4)
            }
                else 
                {
                    SETPINMODE(PINMODE5)
                }
            break;
        }
        case 3:
        {
            if (pinInPort <= 15)
            {
                SETPINMODE(PINMODE6)
            }
                else 
                {
                    SETPINMODE(PINMODE7)
                }
            break;
        }
        case 4:
        {
            if (pinInPort <= 15)
            {
                SETPINMODE(PINMODE8)
            }
                else 
                {
                    SETPINMODE(PINMODE9)
                }
            break;
        }
    }
}
//-------------------------------------------------------------------------------------------------
void commutatePortsAndPullResistors(unsigned char where)
{
unsigned char ii,jj;
    for (ii=0; ii<=4; ii++)        
    {
        for (jj=0; jj<=31; jj++)        
        {
            pinToPullResCommutate(ii,jj,where);
        }
    } 
}
//-------------------------------------------------------------------------------------------------
void hDeinit() 
{
  PCONP &= ~(1<<11);         // external mem controller -> off
  PINSEL6 = 0; // D0- D15 -> ��� ����
//
  commutatePortsAndPullResistors(1);

  PINSEL8 = 0; // A0-A15 -> ��� ����
  PINSEL9 = 0; // A23-A16,OE, BLS0 -> ��� ����

}
//-------------------------------------------------------------------------------------------------
extern "C"
{
    void vPortASDEntry(void); // ��� ����� ����� intASD()
}
#endif // DEVICE_EMULATION
//
void ush_init(void)
{
USHORT *r;
DWORD i;

  vSemaphoreCreateBinary(s_ascan);
  vSemaphoreCreateBinary(s_asd);
  vSemaphoreCreateBinary(s_ImitDP);
#ifndef AC_dis
  vSemaphoreCreateBinary(s_ACTH);
#endif
  varsinit();
//
#ifndef DEVICE_EMULATION
//
#ifndef LEVEL_SENSITIVE_PLD_INT_SERVICE
    EXTMODE = 8; // edge sensitive EINT3
#endif

#ifndef DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
    installirq1( TIMER1_INT, (void *)vPortASDEntry, TIMER1_INT_PRIO,FALSE);
    installirq1( EINT3_INT, (void *)intPLD, EINT2_INT_PRIO,FALSE);
#else
    installirq1( EINT3_INT, (void *)vPortASDEntry, EINT2_INT_PRIO,FALSE);
#endif

    PINSEL4 &= ~(3<<26);
    PINSEL4 |= (1<<26); // EINT3 - P2.13
#endif
//
   Wr_RegPLD(a0x1300,0x101);

  for (i=ExtRamStartAdr;i<ramstart+(0xD000<<1);i+=2)
  {
//    r=(USHORT*)(i);
//    *r= 0;
      Wr_RegPLD(i, 0);
  }
//
   Wr_RegPLD(a0x1300,0);
//
#ifndef DEVICE_EMULATION
#ifdef CONFIGURATION_PRINT
    simplePrintf("\n Hardware DP procession");
#endif
//
#ifndef DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
 PCONP |= (1<<2);   // PCTIM1
 T1MR0 = t1MinimumMatch;
 T1MCR = 7;  // reset & stop
#endif
//
  setRelayCtrlSignals(KSignalsOff);

#ifdef LARGE_BSCAN
  if (xTaskCreate(moveLargeBScan, "mvBScan", configMINIMAL_STACK_SIZE,NULL,TASK_MAX_PRIORITY,&h_moveLargeBScan) == false)
            simplePrintf("\nush_init: error - moveLargeBScan task was not created ");
#endif
#endif //DEVICE_EMULATION
}

