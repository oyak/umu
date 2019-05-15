//
#include "ndbgfea.h"

#ifndef DEVICE_EMULATION

#define disintpld \
{\
    VICINTENCLEAR = (1<<EINT3_INT);\
}
//
// в случае обработки прерывания по уровню бит в EXTINT не будет сброшен,
// если вызывающий прерывания сигнал в активном уровне
#define RESET_INT_PLD \
{\
    EXTINT = 8;\
}

#define PLD_INT_PIN_STATE (FIO2PIN & (1<<13))

#define disintT1 \
{\
  VICINTENCLEAR = (1<<TIMER1_INT);\
}
#define RESET_INT_T1 \
{\
    T1IR = 1;\
}

#define INT_T1_PENDING (T1IR & 1)


#define enintpld  (VICINTENABLE = (1<<EINT3_INT))
#define enintT1  (VICINTENABLE = (1<<TIMER1_INT))

#ifndef DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
#define START_BSCAN_PERIOD \
{\
register unsigned short i;\
    i = 0x80 + ((NumOfTacts & tactbitmsk)<<3);\
    i |= (i << 8);\
    Wr_RegPLD(BScanASDCRegAddr, i);\
    T1TC = 0;\
    T1TCR = 1;\
}
#else
#define START_BSCAN_PERIOD \
{\
register unsigned short i;\
    i = 0x80 + ((NumOfTacts & tactbitmsk)<<3);\
    i |= (i << 8);\
    Wr_RegPLD(BScanASDCRegAddr, i);\
}
#endif
//
#define BSCAN_READY_SIGN_CLEAR \
{\
    Wr_RegPLD(BScanASDCRegAddr, 0x8080);\
}
//
//
#define COND_START_BSCAN_PERIOD_EX(startedFlag) \
{\
    if (fIntPLDMustBeEnable)\
    {\
        START_BSCAN_PERIOD;\
        startedFlag = 1;\
    }\
        else startedFlag = 0;\
}
//
#define WHEN_START_PLD \
{\
    if ((EnableBScan)||(EnableASD))\
    {\
        intdisf();\
        COND_INT_ASD_ENABLE_UNLOCK\
        enintpld;\
        intenf();\
        START_BSCAN_PERIOD\
    }\
    TakeSemaphore(s_ascan);\
    if (isAScanStopped(TRUE)) flWasStopped=0;\
    xSemaphoreGive(s_ascan);\
    Wr_RegPLD(a0x1300, 0x202);\
}

#else //     defined DEVICE_EMULATION

#define RESET_INT_PLD

#define PLD_INT_PIN_STATE

#define disintT1

#define RESET_INT_T1

#define INT_T1_PENDING
#define enintT1


#define START_BSCAN_PERIOD \
{\
\
}
//
#define BSCAN_READY_SIGN_CLEAR \
{\
}

void intdisf()
{
}


void intenf()
{

}
//
#endif
//
//
#define COND_START_BSCAN_PERIOD_EX(startedFlag) \
{\
    if (fIntPLDMustBeEnable)\
    {\
        START_BSCAN_PERIOD;\
        startedFlag = 1;\
    }\
        else startedFlag = 0;\
}
//
#define WHEN_START_PLD \
{\
    if ((EnableBScan)||(EnableASD))\
    {\
        intdisf();\
        COND_INT_ASD_ENABLE_UNLOCK\
        enintpld;\
        intenf();\
        START_BSCAN_PERIOD\
    }\
    TakeSemaphore(s_ascan);\
    if (isAScanStopped(TRUE)) flWasStopped=0;\
    xSemaphoreGive(s_ascan);\
    Wr_RegPLD(a0x1300, 0x202);\
}


unsigned int intASD(void);

#ifndef DEVICE_EMULATION
__irq __arm void intPLD(void);
#endif

const UCHAR mask[8]={1,2,4,8,16,32,64,128};
const UCHAR mask2[8]={0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};
//
//
#ifndef OLD_PACKET_HEADER
// ЇаЁ ўл§®ўҐ par - нв® UCHAR* -  ¤аҐб ­ з «  Ї ЄҐв 
void attachMessageNumber(void * par)
{
   ((UCHAR*)par)[4] = messageNumber;
   messageNumber++;
}
//
#define ATTACH_MESSAGE_NUMBER(a) \
{ \
        a = messageNumber;\
        messageNumber++;\
}
#endif
//-------------------------------------------------------------------
inline void attachMessageNumberToHeader(tLANMESSAGEHEADER *pHdr)
{
   pHdr->MessageCount = messageNumber;
   messageNumber++;
}
//-------------------------------------------------------------------
inline void attachMessageLengthToHeader(tLANMESSAGEHEADER *pHdr,unsigned short len)
{
   pHdr->Size = len;
}
//-------------------------------------------------------------------
#ifndef DEVICE_EMULATION
#define GET_DPx_VALUE(DPNumber) (*(pDPValue[DPNumber]) >> 8)
#else
unsigned char GET_DPx_VALUE(unsigned char DPNumber)
{
    if (DPNumber == 0) return (Rd_RegPLD(LENGTHDPVALUEADR) >> 8);
        else return 0;
}
#endif
//-------------------------------------------------------------------
inline unsigned char getSideIndex(unsigned char *pByte)
{
   return  *pByte >> 7;
}
//-------------------------------------------------------------------
inline unsigned char getLineIndex(unsigned char *pByte)
{
  return (*pByte & linebitmsk) >> 6;
}
//-------------------------------------------------------------------
inline unsigned char getTacktNumber(unsigned char *pByte)
{
  return *pByte &  tactbitmsk;
}
//-------------------------------------------------------------------
void condIntASDEnable(void)
{
    COND_IntASD_ENABLE
}
//-------------------------------------------------------------------

void restartAScan(void)
{
register unsigned char i;
    for (i = 0; i < 4; ++i)
    {
      if (ascan[i] > 1) ascan[i] = 1;
    }
}
//-------------------------------------------------------------------
#ifndef DEVICE_EMULATION
void printTactParams(unsigned char tact)
{
unsigned int jj;
unsigned short *BA;
unsigned char ii;

    tact &=  tactbitmsk;
    simplePrintf("\nTact %d Parameters:", tact);
    jj = parreg_sz * tact + ExtRamStartAdr;

    BA = (USHORT*)(jj + _ChLCRmask);
    simplePrintf("\nSide 0 line 1, gen-rcvr - 0x%x", *BA & 0xFF);
    simplePrintf("\nSide 1 line 1, gen-rcvr - 0x%x", *BA >> 8);

    BA++;
    simplePrintf("\nSide 0 line 0, gen-rcvr - 0x%x", *BA & 0xFF);
    simplePrintf("\nSide 1 line 0, gen-rcvr - 0x%x", *BA >> 8);
//
    BA = (USHORT*)(jj + _TprizmL1mask);
    simplePrintf("\nSide 0 line 1, Tpizm Low Byte  - %d", *BA & 0xFF);
    simplePrintf("\nSide 1 line 1, Tpizm Low Byte  - %d", *BA >> 8);
//
    BA = (USHORT*)(jj + _TprizmL2mask);
    simplePrintf("\nSide 0 line 1, Tpizm High Byte  - %d", *BA & 0xFF);
    simplePrintf("\nSide 1 line 1, Tpizm High Byte  - %d", *BA >> 8);
//
    BA = (USHORT*)(jj + _TprizmR1mask);
    simplePrintf("\nSide 0 line 0, Tpizm Low Byte  - %d", *BA & 0xFF);
    simplePrintf("\nSide 1 line 0, Tpizm Low Byte  - %d", *BA >> 8);
//
    BA = (USHORT*)(jj + _TprizmR2mask);
    simplePrintf("\nSide 0 line 0, Tpizm High Byte  - %d", *BA & 0xFF);
    simplePrintf("\nSide 1 line 0, Tpizm High Byte  - %d", *BA >> 8);
//
    BA = (USHORT*)(jj + _WSALmask);
    simplePrintf("\nSide 0 line 1, WSA Low Byte  - %d", *BA & 0xFF);
    simplePrintf("\nSide 1 line 1, WSA Low Byte  - %d", *BA >> 8);
//
    BA++;
    simplePrintf("\nSide 0 line 0, WSA High Byte  - %d", *BA & 0xFF);
    simplePrintf("\nSide 1 line 0, WSA High Byte  - %d", *BA >> 8);
//
    BA = (USHORT*)(jj + _RazvCRmask);
    simplePrintf("\nSide 0 AScan Duration - %d", *BA & 0xFF);
    simplePrintf("\nSide 1 AScan Duration - %d", *BA >> 8);
//
    BA = (USHORT*)(jj + _LevStrL0mask);
    for (ii=0; ii<4; ++ii)
    {
        simplePrintf("\nSide 0 line 1, gate %d level  - %d", ii, *BA & 0xFF);
        simplePrintf("\nSide 1 line 1, gate %d level  - %d", ii, *BA >> 8);
        BA++;
    }
//
    BA = (USHORT*)(jj + _LevStrR0mask);
    for (ii=0; ii<4; ++ii)
    {
        simplePrintf("\nSide 0 line 0, gate %d level  - %d", ii, *BA & 0xFF);
        simplePrintf("\nSide 1 line 0, gate %d level  - %d", ii, *BA >> 8);
        BA++;
    }
//
    BA = (USHORT*)(jj + _AMPZI1mask);
    simplePrintf("\nSide 0 line 1 ZI amplitude - %d", *BA & 0xFF);
    simplePrintf("\nSide 1 line 1 ZI amplitude- %d",  *BA >> 8);
//
    BA++;
    simplePrintf("\nSide 0 line 0 ZI amplitude - %d", *BA & 0xFF);
    simplePrintf("\nSide 1 line 0 ZI amplitude - %d", *BA >> 8);
//
    BA = (USHORT*)(jj + _FreqL2mask);
    if ((*BA & 1) == 0) simplePrintf("\nSide 0 line 1 ZI - 2.5 MHZ");
        else simplePrintf("\nSide 0 line 1 ZI - 5 MHZ");
    if ((*BA & 0x100) == 0) simplePrintf("\nSide 1 line 1 ZI - 2.5 MHZ");
        else simplePrintf("\nSide 1 line 1 ZI - 5 MHZ");
//
    BA = (USHORT*)(jj + _FreqL1mask);
    if ((*BA & 1) == 0) simplePrintf("\nSide 0 line 0 ZI - 2.5 MHZ");
        else simplePrintf("\nSide 0 line 1 ZI - 5 MHZ");
    if ((*BA & 0x100) == 0) simplePrintf("\nSide 1 line 0 ZI - 2.5 MHZ");
        else simplePrintf("\nSide 1 line 1 ZI - 5 MHZ");
//
    BA = (USHORT*)(jj + BScanCutStartFrac2mask);
    if ((*BA & (1<<BScanSignalsCutOnParamBitNum)) == 0) simplePrintf("\nSide 0 line 1 BScan cutting OFF");
        else
        {
            unsigned char frac = (unsigned char)(*BA & BScanSignalsCutStartFracParamBitMask);
            BA = (USHORT*)(jj + BScanCutStart2mask);
            simplePrintf("\nSide 0 line 0 BScan cutting ON, start time - %d.%d us", *BA & 0xFF, frac);
        }
//
    BA = (USHORT*)(jj + BScanCutStartFrac2mask);
    if ((*BA>>8) & (1<<BScanSignalsCutOnParamBitNum) == 0) simplePrintf("\nSide 1 line 1 BScan cutting OFF");
        else
        {
            unsigned char frac = (unsigned char)((*BA>>8) & BScanSignalsCutStartFracParamBitMask);
            BA = (USHORT*)(jj + BScanCutStart2mask);
            simplePrintf("\nSide 1 line 1 BScan cutting ON, start time - %d.%d us", *BA >> 8, frac);

        }
    //
        BA = (USHORT*)(jj + BScanCutStartFrac1mask);
        if ((*BA & (1<<BScanSignalsCutOnParamBitNum)) == 0) simplePrintf("\nSide 0 line 0 BScan cutting OFF");
            else
            {
                unsigned char frac = (unsigned char)(*BA & BScanSignalsCutStartFracParamBitMask);
                BA = (USHORT*)(jj + BScanCutStart1mask);
                simplePrintf("\nSide 0 line 0 BScan cutting ON, start time - %d.%d us", *BA & 0xFF, frac);
            }
    //
        BA = (USHORT*)(jj + BScanCutStartFrac1mask);
        if ((*BA>>8) & (1<<BScanSignalsCutOnParamBitNum) == 0) simplePrintf("\nSide 1 line 0 BScan cutting OFF");
            else
            {
                unsigned char frac = (unsigned char)((*BA>>8) & BScanSignalsCutStartFracParamBitMask);
                BA = (USHORT*)(jj + BScanCutStart1mask);
                simplePrintf("\nSide 1 line 0 BScan cutting ON, start time - %d.%d us", *BA >> 8, frac);

            }
}
#else
void printTactParams(unsigned char tact)
{}
#endif
//-------------------------------------------------------------------
// ў®§ўа й Ґв 0, Ґб«Ё ўбҐ ¤ ­­лҐ Ўл«Ё § ЇЁб ­л, Ё­ зҐ 1
//
// line = 0/1  (liniya1/liniya2)
void read_ascan(UCHAR line)
{
USHORT a,a1,i;
#ifndef DEVICE_EMULATION
USHORT *AScanAddr;
#else
unsigned int AScanAddr;
#endif
register USHORT k; // USHORT - !!!
BOOL frep;

//     smprintf("read_ascan: line = %d",line+1);

#ifdef BUG_SEARCH
   if (ascan[line] == 0) && (ascan[line+2] == 0)
   {
//     smprintf("read_ascan: false call line = %d",line+1);
     return 0;
   }
#endif
//
   TakeSemaphore(s_ascan);
#ifndef DEVICE_EMULATION
   if (!line) AScanAddr = (USHORT*)Aaddr1; // liniya 1
         else  AScanAddr = (USHORT*)Aaddr2;  // liniya 2
#else
   if (!line) AScanAddr = Aaddr1;
         else  AScanAddr = Aaddr2;
#endif
//
  do
  {
   frep = FALSE;
   switch (ascan[line] | ascan[line+2])
   {
   case 1:
   {  // even if one of the ascan[]`s pair equals 0
       if (  (ascan[line] & ascanregim[line] & 0x80 ) || (ascan[line+2] & ascanregim[line+2] & 0x80 ) || (get_tickdur(AscanSendTick[line]) > TimeToTick(AscanSendTime) ) )
       { // ("hotya by odna iz razvertok odnokratnaya") or ("pauza istekla dly ciclicheskoi")
//         smprintf("\n ascan starting - line %d",line);
         a=ascanscale[line] | ascanscale[line+2] << 8;

//         *(USHORT*)((DWORD)AScanAddr+(0xFE<<1)) = a;
          Wr_RegPLD(AScanAddr+(0xFE<<1), a);

         a=ascanstart[line] | ascanstart[line+2] << 8;
//         *(USHORT*)((DWORD)AScanAddr+(0xFD<<1)) = a;
          Wr_RegPLD(AScanAddr+(0xFD<<1), a);
//
         a=curchan[line] | curchan[line+2] << 8;
         if (line) a |= 0x8080;
//         *(USHORT*)((DWORD)AScanAddr+(0xFC<<1))=a;
          Wr_RegPLD(AScanAddr+(0xFC<<1), a);
//
         a=(1+(ascanstrobformax[line]<<2) ) | ( (1+ (ascanstrobformax[line+2]<<2)) << 8);
         a1=((ascanregim[line] & 0xC) << 3) | (((ascanregim[line+2] & 0xC) << 3) << 8) ;  //
         a1=a1 | a;
//         *(USHORT*)((DWORD)AScanAddr+(0xFF<<1)) = a1;
          Wr_RegPLD(AScanAddr+(0xFF<<1), a1);

//   mozhno li ne zapuskat A-razv esli sootvetstv. ascan[] == 0
//
         if (ascan[line]) ascan[line] = 2;
         if (ascan[line+2]) ascan[line+2] = 2;
//
         AscanSendTick[line] = xTaskGetTickCount();
       }
       break;
   }
//
   case 0x2:
   case 0x3:
   case 0x6:
   {
//
       Set_CSPLD;
//       i = *(USHORT*)((DWORD)AScanAddr+(0xFF<<1));
       i = Rd_RegPLD(AScanAddr+ (0xFF<<1));
       Reset_CSPLD;
//
       if ((i & 0x101) == 0)
       {
         if (ascan[line] == 2)  ascan[line] = 4;
         if (ascan[line+2] == 2) ascan[line+2] = 4;
//         frep = TRUE;  go further
//         break;
       }
        else
        {
         if (flWasStopped != 0)
         { // если останавливали работу ПЛИС
             restartAScan();
             flWasStopped = 0;
             break;
         }

         if (get_tickdur(AscanSendTick[line]) > (TimeToTick(AscanSendTime) >> 2))
         {
           if ((ascan[line] == 2) && ((i & 0x1) == 0))
           { // L-ascan  ready and wanted
            if (!(ascanregim[line] & 0x10))
            {
//              smprintf("\n L-ascan line %d is ready",line);
              fill0x7F_hdr(&tempbuf[0])
              for (k = 0; k< 232; ++k)
              {

#ifdef ADC_CONST
                tempbuf[asbdy_offs+k]  = (ADC_CONST_VALUE & 0xFF);
#else
                Set_CSPLD;
//                tempbuf[asbdy_offs+k] = *AScanAddr & 0xFF;
                tempbuf[asbdy_offs+k] = Rd_RegPLD(AScanAddr) & 0xFF;
                Reset_CSPLD;
//                AScanAddr++;
                AScanAddr +=2;
#endif
              }
              tempbuf[slt_offs] = curchan[line] | (line << 6);

#ifdef OLD_PACKET_HEADER
              putmsg(tempbuf,szArazv+hdrsize,NULL);
#else
              putmsg(tempbuf,szArazv+hdrsize,attachMessageNumber);
#endif
            }
//
#ifndef msg0x82_dis
              fill0x82_hdr(&tempbuf[0])
#ifndef DEVICE_EMULATION
              if (!line) AScanAddr = (USHORT*)(Aaddr1 + (0xE8 << 1)); // liniya 1
                     else  AScanAddr = (USHORT*)(Aaddr2 + (0xE8 << 1));  // liniya 2
#else
              if (!line) AScanAddr = Aaddr1 + (0xE8 << 1);
                  else  AScanAddr = Aaddr2 + (0xE8 << 1);
#endif
              tempbuf[slt_offs] = curchan[line] | (line<<6);
#ifdef OLD_PACKET_HEADER
              for (k = 5; k< 8; ++k)
#else
              for (k = 7; k< 10; ++k)
#endif
              {
                 Set_CSPLD;
//                 tempbuf[k] = (UCHAR)*AScanAddr;
                 tempbuf[k] = Rd_RegPLD(AScanAddr) & 0xFF;
                 Reset_CSPLD;
//                AScanAddr++;
                 AScanAddr += 2;
              }
#ifdef OLD_PACKET_HEADER
              putmsg(tempbuf,szAAZ+hdrsize,NULL);
#else
              putmsg(tempbuf,szAAZ+hdrsize,attachMessageNumber);
#endif

#endif
//
              ascan[line] = 8;
              frep = TRUE;
              break;
           }
           if ((ascan[line+2] == 2) && ((i & 0x100) == 0))
           {  // R-ascan  ready and wanted
            if (!(ascanregim[line+2] & 0x10))
            {
//              smprintf("\n R-ascan line %d is ready",line);
              fill0x7F_hdr(&tempbuf[(tempbuf_size>>1)])
              for (k = 0; k< 232; ++k)
              {

#ifdef ADC_CONST
              tempbuf[(tempbuf_size>>1)+asbdy_offs+k]  =  ADC_CONST_VALUE >> 8;
#else

                Set_CSPLD;
//                tempbuf[(tempbuf_size>>1)+asbdy_offs+k] = *AScanAddr >> 8;
                tempbuf[(tempbuf_size>>1)+asbdy_offs+k] = Rd_RegPLD(AScanAddr) >> 8;
                Reset_CSPLD;
#endif
//                AScanAddr++;
                AScanAddr += 2;
              }
              tempbuf[(tempbuf_size>>1)+slt_offs] = curchan[line+2] | (line<<6) | 0x80;  // right hand side

#ifdef OLD_PACKET_HEADER
              putmsg(&tempbuf[tempbuf_size>>1],szArazv+hdrsize,NULL);
#else
              putmsg(&tempbuf[tempbuf_size>>1],szArazv+hdrsize,attachMessageNumber);
#endif

            }
#ifndef msg0x82_dis
             fill0x82_hdr(&tempbuf[(tempbuf_size>>1)])
#ifndef DEVICE_EMULATION
              if (!line) AScanAddr = (USHORT*)(Aaddr1 + (0xE8 << 1)); // liniya 1
                     else  AScanAddr = (USHORT*)(Aaddr2+ (0xE8 << 1));  // liniya 2
#else
              if (!line) AScanAddr = Aaddr1 + (0xE8 << 1);
                  else  AScanAddr = Aaddr2+ (0xE8 << 1);
#endif

              tempbuf[(tempbuf_size>>1)+slt_offs] = curchan[line+2] | (line<<6) | 0x80;  // right hand side
#ifdef OLD_PACKET_HEADER
              for (k = 5; k< 8; ++k)
#else
              for (k = 7; k< 10; ++k)
#endif
              {
                 Set_CSPLD;
//                 tempbuf[k+(tempbuf_size>>1)] = *AScanAddr >> 8;
                 tempbuf[k+(tempbuf_size>>1)] = Rd_RegPLD(AScanAddr) >> 8;
                 Reset_CSPLD;
//                 AScanAddr++;
                 AScanAddr +=2;
              }

#ifdef OLD_PACKET_HEADER
              putmsg(&tempbuf[tempbuf_size>>1],szAAZ+hdrsize,NULL);
#else
              putmsg(&tempbuf[tempbuf_size>>1],szAAZ+hdrsize,attachMessageNumber);
#endif

#endif
//
             ascan[line+2] = 8;
             frep = TRUE;
             break;
           }
// error message if not ready
           simplePrintf("\nread_ascan: line %d - Ascan is NOT ready",line);
           restartAScan();
         }
         break;
        } // else
   }
//
// сюда ничего не вставлять - должны провалиться из предыдущего case
//
   case 0x4:
   { // both sides are ready
//   smprintf("\nascan: line %d  2 side scan is  ready",line);
      fill0x7F_hdr(&tempbuf[0])
      fill0x7F_hdr(&tempbuf[(tempbuf_size>>1)])
      for (k = 0; k< 232; ++k)
      {
         Set_CSPLD;
#ifdef ADC_CONST
         i = (ADC_CONST_VALUE & 0xFF) | ADC_CONST_VALUE << 8;
#else
//         i = *AScanAddr;
         i = Rd_RegPLD(AScanAddr);
#endif

         Reset_CSPLD;
         tempbuf[asbdy_offs+k] = i & 0xFF;  // Left ascan
         tempbuf[(tempbuf_size>>1)+asbdy_offs+k] = i >> 8;  // Right ascan
//         AScanAddr++;
          AScanAddr +=2;
      }

      if (ascan[line])
      {
        if (!(ascanregim[line] & 0x10))   
        {
           tempbuf[slt_offs] = curchan[line] | (line<<6) ;  // left hand side

#ifdef OLD_PACKET_HEADER
           putmsg(tempbuf,szArazv+hdrsize,NULL);
#else
           putmsg(tempbuf,szArazv+hdrsize,attachMessageNumber);
#endif

        }
        ascan[line] = 8;
      }
      if (ascan[line+2])
      {
       if (!(ascanregim[line+2] & 0x10))  
       {
           tempbuf[(tempbuf_size>>1)+slt_offs] = curchan[line+2] | (line<<6) | 0x80;  // right hand side

#ifdef OLD_PACKET_HEADER
           putmsg(&tempbuf[tempbuf_size>>1],szArazv+hdrsize,NULL);
#else
           putmsg(&tempbuf[tempbuf_size>>1],szArazv+hdrsize,attachMessageNumber);
#endif
       }
       ascan[line+2] = 8;
      }
//
#ifndef msg0x82_dis
      if (ascan[line] | ascan[line+2])
      {
        fill0x82_hdr(&tempbuf[0])
        fill0x82_hdr(&tempbuf[(tempbuf_size>>1)])
#ifndef DEVICE_EMULATION
        if (!line) AScanAddr = (USHORT*)(Aaddr1 + (0xE8 << 1)); // liniya 1
             else  AScanAddr = (USHORT*)(Aaddr2 + (0xE8 << 1));  // liniya 2
#else
        if (!line) AScanAddr = Aaddr1 + (0xE8 << 1);
            else  AScanAddr = Aaddr2 + (0xE8 << 1);
#endif
        tempbuf[slt_offs] = curchan[line] | (line<<6) ;  // left hand side
        tempbuf[(tempbuf_size>>1)+slt_offs] = curchan[line+2] | (line<<6) | 0x80;  // right hand side

#ifdef OLD_PACKET_HEADER
              for (k = 5; k< 8; ++k)
#else
              for (k = 7; k< 10; ++k)
#endif
              {
                 Set_CSPLD;
//                 a = *AScanAddr++;
                 a = Rd_RegPLD(AScanAddr);
                 AScanAddr +=2;
                 Reset_CSPLD;
                 tempbuf[k] = (UCHAR)a;
                 tempbuf[k+(tempbuf_size>>1)] = a >> 8;
              }

#ifdef OLD_PACKET_HEADER
              if (ascan[line]) putmsg(&tempbuf[0],szAAZ+hdrsize,NULL);
              if (ascan[line+2]) putmsg(&tempbuf[tempbuf_size>>1],szAAZ+hdrsize,NULL);
#else
              if (ascan[line]) putmsg(&tempbuf[0],szAAZ+hdrsize,attachMessageNumber);
              if (ascan[line+2]) putmsg(&tempbuf[tempbuf_size>>1],szAAZ+hdrsize,attachMessageNumber);
#endif

      }
#endif
// go further
//      frep = TRUE;
//      break;
   }
//
// сюда ничего не вставлять - должны провалиться из предыдущего case
   case 0x8:
   case 0x9:
   case 0xA:
   {
      for (i=0; i<2; ++i)
      {
        k = line+(i<<1);
        if (ascan[k] > 1)
        {
           if ((ascanregim[k] & 0x80) == 0)
           {
              if (ascanregim[k] & 0x40)
              {
                 curchan[k]++;
                 if (curchan[k]==NumOfTacts) curchan[k] = 0;
              }
              ascan[k] = 1;
           }
             else ascan[k] = 0; // odnokr.regim
        }
      }
      break;
   }
   }
   } while (frep);
//
   xSemaphoreGive(s_ascan);
}
//-------------------------------------------------------------------
// д®а¬ЁагҐв ®вўҐв б ­ Їап¦Ґ­ЁҐ¬  ЄЄг¬г«пв®а 
// UЁ§¬/Uakk = 332/3632 - б¬.¤Ґ«ЁвҐ«м
// ®Ї®а­®Ґ ­ ЇаҐ¦Ґ­ЁҐ Ђ–Џ ЇаҐ¤Ї®« Ј Ґвбп а ў­л¬ 3,3‚,
// Ґ¬г б®®вўҐвбвўгҐв зЁб«® 1023
void Get_Uakk(void)
{
DWORD u = get_msrd() *1198560 / 3396360; // ў ¤Ґбпвле ¤®«пе ў®«мв 
  O1Data[0] = idVolt;
  O1Data[1] = idBUM;
  O1Data[2]  = szVolt;
  O1Data[3]  = 0;
//
  O1Data[hdrsize]  = u/10;
  O1Data[hdrsize+1]  = u%10;

#ifdef OLD_PACKET_HEADER
  putmsg(O1Data,szVolt+hdrsize,NULL);
#else
  O1Data[5]  = 0;
  putmsg(O1Data,szVolt+hdrsize,attachMessageNumber);
#endif
}
//-------------------------------------------------------------------
void PLDEn(void){;}
//
void PDLDis(void){;}
//-------------------------------------------------------------------
void ReloadBUM(void){;}
//-------------------------------------------------------------------
//!
void SetNumOfTacts(UCHAR *ptr)
{
register USHORT a;

  smprintf("\nSetNumOfTacts enter");

  Stop_PLD_Work();

  TBD

#ifndef LARGE_BSCAN   
  StartBScanFIFO=EndBScanFIFO;
#endif  

  NumOfTacts=*ptr;
  a=NumOfTacts-1;
  a = a | (a<<8);
  Wr_RegPLD(a0x1301, a);
  Start_PLD_Work();
}
//-------------------------------------------------------------------
BOOL isAScanStopped(BOOL fUnlocked)
{
register BOOL res = TRUE;
register unsigned char ii;


  if (fUnlocked == FALSE) TakeSemaphore(s_ascan);
  for (ii=0; ii<4; ++ii)
  {
      if (ascan[ii] != 0)
      {
          res = FALSE;
          break;
      }
  }
  if (fUnlocked == FALSE) xSemaphoreGive(s_ascan);
  return res;
}

//-------------------------------------------------------------------
void StopAScan(void)
{
register UCHAR ii;

  TakeSemaphore(s_ascan);
  for (ii=0; ii<4; ++ii) ascan[ii] = 0;
//  jj = ascancounter;
  xSemaphoreGive(s_ascan);
//
/*
  O1Data[0] = 0x46;
  O1Data[1] = idBUM;
  O1Data[2]  = 4;
  O1Data[3]  = 0;
#ifndef OLD_PACKET_HEADER
   O1Data[5] = 0;
#endif
//
  WriteLE32U(&O1Data[hdrsize],jj);
#ifdef OLD_PACKET_HEADER
  putmsg(O1Data,4+hdrsize,NULL);
#else
  putmsg(O1Data,4+hdrsize,attachMessageNumber);
#endif
//
  simplePrintf("\n %d ascan were sent",jj);
*/
}
//-------------------------------------------------------------------
void StartBScan(void)
{
  smprintf("\nStartBScan enter");

  if ((mainShiftSensorNumber & IMITATORMASK) == 0)
  { // так как основной ДП могли покрутить, достаточно 2х чтений, но делаем 3
// подобно сделано в doWhenImitSwitchOFF(), но здесь п.п с проверками на номер ДП
  register unsigned char ii;
      for(ii=0; ii<3; ++ii)
      {
          getShiftSensorValue(mainShiftSensorNumber);
          delay(10);
      }
      mainShiftSensorPathcoordMemorized = Pathcoord[mainShiftSensorNumber];
      speedTimer = xTaskGetTickCount();
  }

#ifdef LARGE_BSCAN
  initMScanFlags();
  Wr_RegPLD(ACControlReg, ACControlValue_EVAL);
  Wr_RegPLD(ACSumStartReg, ACSumStartValue);
#else
    fMScan = 0;
    fMScan1 = 0;
   MScanSendTick = xTaskGetTickCount();
#endif    
  EnableBScan=1;
  Start_PLD_Work();
}
//-------------------------------------------------------------------
void StopBScan(void)
{
  smprintf("\nStopBScan enter");
  EnableBScan=0;
  ASD_Off();
#ifndef AC_dis
  acMessageOff();
#endif
  Stop_PLD_Work();
//
// Ї®¤вўҐа¦¤Ґ­ЁҐ ўлЇ®«­Ґ­Ёп Є®¬ ­¤л
  O1Data[0] = 0x48;
  O1Data[1] = idBUM;
  O1Data[2]  = 0;
  O1Data[3]  = 0;
//
#ifdef OLD_PACKET_HEADER
  putmsg(O1Data,hdrsize,NULL);
#else
  O1Data[5] = 0;
  putmsg(O1Data,hdrsize,attachMessageNumber);
#endif
}
//
//-------------------------------------------------------------------
//!
void Stop_PLD_Work(void)
{
//  smprintf("\nStop_PLD_Work enter");
    TakeSemaphore(s_ascan);
    Wr_RegPLD(a0x1300, 0); // сигнал прерывания станет неактивным
    flWasStopped=1;
    xSemaphoreGive(s_ascan);

    intdisf(); // чтобы исключить переключение задач

    BSCAN_READY_SIGN_CLEAR

#ifndef DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
    disintT1
    RESET_INT_T1
#endif
    disintpld
    RESET_INT_PLD
    COND_INT_ASD_ENABLE_LOCK
    intenf();
}
//-------------------------------------------------------------------
void Start_PLD_Work(void)
{
//    smprintf("\nStart_PLD_Work enter");
    Wr_RegPLD(a0x1300, 0); // PLD registers may not be accessble
// пока прерывания от ПЛИС невозможны
#ifndef AC_dis
    TakeSemaphore(s_ACTH);
    if (ACCalcStage == ACCALC_WAITFORDATA)
    {
        ACCalcStage = ACCALC_NEED_TO_START;
    }
        else if (ACCalcStage == ACTHCALC_WAITFORDATA)
             {
                 ACCalcStage = ACTHCALC_CNTQUERY;
             }
    xSemaphoreGive(s_ACTH);
#endif
    WHEN_START_PLD
}
//
//-------------------------------------------------------------------
void StartPLD(void)
{
//    smprintf("\nStartPLD enter");
    Wr_RegPLD(a0x1300, 0); // PLD registers may not be accessble
// пока прерывания от ПЛИС невозможны
    WHEN_START_PLD
}
//
//-------------------------------------------------------------------
unsigned char correctDACValue(unsigned char value)
{
if (value < c_dacminval) value = c_dacminval;
    else if (value > c_dacmaxval) value = c_dacmaxval;
    return  value + c_dacCorrection;
}
//-------------------------------------------------------------------
void SetTactParameter(unsigned char tactNumber, unsigned int parameterOffset, unsigned short parameterValue)
{

}
//-------------------------------------------------------------------
// очиститка области параметров такта не производится, т.к.
// п.п может вызываться, когда B-развертка включена, а здесь время
// в призме не устанавливается
void SetTacktParam(tTACTPARAMSCMD *pData)
{
unsigned short *BA;
unsigned short a;
unsigned short b;
unsigned short ii;
unsigned char takt;
DWORD jj;
unsigned char *p;

  takt = pData->TacktNumber;  // takt number 0...
  
  smprintf("\nSetTaktParam enter: takt = %d",takt);

  if (takt >= (tactbitmsk+1))
  {
      simplePrintf("\nSetTaktParam: takt number is wrong - %d", takt);
      return;
  }

  Stop_PLD_Work();
  Wr_RegPLD(a0x1300, 0x101);

/*
  simplePrintf("\nSetTaktParam: pData->ACATTLn1L - %d", pData->ACATTLn1L);
  simplePrintf("\nSetTaktParam: pData->ACATTLn1R - %d", pData->ACATTLn1R);
  simplePrintf("\nSetTaktParam: pData->ACATTLn2L - %d", pData->ACATTLn2L);
  simplePrintf("\nSetTaktParam: pData->ACATTLn2R - %d", pData->ACATTLn2R);
*/

  if (pData->Duration > MaxAScanDuration) pData->Duration = MaxAScanDuration; // ограничить длительность развертки

#ifndef DEVICE_EMULATION


  jj = parreg_sz * takt +ExtRamStartAdr;
//
  if (!takt)
  {
     BA=(USHORT*)(jj + _WSALmask); // offset WSALmask
     *BA++ = 0;
     a = extramstart + parreg_sz * NumOfTacts; // low byte = 0 here

      smprintf("\nSetTacktParam: takt 0, WSA = %x", a);

     a = a | (a >> 8);
     *BA=a;

//очистить рабочую область такта, считая ее максимально длинной, если
// B-развертка не включена
    if (EnableBScan == 0)
    {
        BA = (USHORT*)( (a & 0xFF00) + ramstart) ;
        for(b=0; b<256*4; ++b) BA[b] = 0;
        for(b=0; b < AContactZone; ++b)
        {
            BA[((b + pData->Duration) << 2) + 1] = (correctDACValue(pData->ACATTLn1L) << 8) | correctDACValue(pData->ACATTLn1R);
            BA[((b + pData->Duration) << 2) + 2] = (correctDACValue(pData->ACATTLn2L) << 8) | correctDACValue(pData->ACATTLn2R);
        }
    }
//     smprintf("\nSetTacktParam: takt - wraddr = %x", BA);
  }
    else
    {
         BA=(USHORT*)( parreg_sz * (takt-1)  + ExtRamStartAdr + _WSALmask); // offset WSALmask v parametrah predydushego takta
         b = *BA++;
         a = *BA;

          if ( (b == 0) && (a != 0) )
          {
              BA=(USHORT*)(parreg_sz * (takt-1) + ExtRamStartAdr + _RazvCRmask);
              a <<= 8;
              b = *BA + AContactZone; // не должно превысить 255
              a += ((b & 0xFF) * 4 + 100) << 1;  // "<<1", i.e word addressing
              if (a & 0xFF) a += 0x100; // ceiling
              a &= 0xFF00;

              smprintf("\nSetTacktParam: takt %d, WSA = %x", takt,a);

              a = a | (a>>8);
              BA=(USHORT*)(jj + _WSALmask); // offset WSALmask
              *BA++ = 0;
              *BA = a;
//
//очистить рабочую область такта, считая ее максимально длинной, если
// B-развертка не включена
             if (EnableBScan == 0)
             {
                 BA = (USHORT*)( (a & 0xFF00) + ramstart) ;
                 for(b=0; b<256*4; ++b) BA[b] = 0;
                 for(b=0; b < AContactZone; ++b)
                 {
                     BA[((b + pData->Duration) << 2) + 1] = (correctDACValue(pData->ACATTLn1L) << 8) | correctDACValue(pData->ACATTLn1R);
                     BA[((b + pData->Duration) << 2) + 2] = (correctDACValue(pData->ACATTLn2L) << 8) | correctDACValue(pData->ACATTLn2R);
                 }
             }
         }
            else
            { // WSA takta ii-1 ne opredelen
              simplePrintf("\nSetTacktParam: takt's %d WSA not defined and HiByte = %x, LoByte = %x",takt-1, a,b);
            }
         }
//
  BA=(USHORT*)(jj + _ChLCRmask); // offset ChLCRmask

#ifndef AC_dis
  if ((pData->GenRcvLn1L & 0x0F) < NumberOfReceivers)
  {
          receiversInCycle[indexLeft][0][takt] = pData->GenRcvLn1L & 0x0F;
  }
      else
      {
          receiversInCycle[indexLeft][0][takt] = RECEIVER_UNKNOWN;
      }
  if ((pData->GenRcvLn1R & 0x0F) < NumberOfReceivers)
  {
          receiversInCycle[indexRight][0][takt] = pData->GenRcvLn1R & 0x0F;
  }
      else
      {
          receiversInCycle[indexRight][0][takt] = RECEIVER_UNKNOWN;
      }
#endif
  a = pData->GenRcvLn1R | (pData->GenRcvLn1L << 8);  // liniya 1
//
//  smprintf("\nSetTacktParam: line 1, genr-rcvr - genl-rcvl = %x", a);
//
  *BA++=a;
//
#ifndef AC_dis
  if ((pData->GenRcvLn2L & 0x0F) < NumberOfReceivers)
  {
          receiversInCycle[indexLeft][1][takt] = pData->GenRcvLn2L & 0x0F;
  }
      else
      {
          receiversInCycle[indexLeft][1][takt] = RECEIVER_UNKNOWN;
      }
  if ((pData->GenRcvLn2R & 0x0F) < NumberOfReceivers)
  {
          receiversInCycle[indexRight][1][takt] = pData->GenRcvLn2R & 0x0F;
  }
      else
      {
          receiversInCycle[indexRight][1][takt] = RECEIVER_UNKNOWN;
      }
#endif
  a = pData->GenRcvLn2R | (pData->GenRcvLn2L << 8);  // liniya 2
//
//  smprintf("\nSetTacktParam: line 2, genr-rcvr - genl-rcvl = %x", a);
//
  *BA=a;
//
  BA=(USHORT*)(jj + _RazvCRmask); // offset RazvCRmask
//
  a =  pData->Duration;
  a =  a | (a << 8);
  *BA=a;    
  smprintf("\nSetTacktParam: AScan length = 0x%x", a);
//
// з бв®в  ‡€ «Ё­Ёп 1,2
  {
    BA=(USHORT*)(jj + _FreqL1mask); 
    *BA = ((pData->PulseFreqL & 1) << 8)  |  (pData->PulseFreqR & 1);   // skrestit lev i prav storony
    smprintf("\nSetTacktParam: line's 1  Freq value = 0x%x",*BA);
    BA=(USHORT*)(jj + _FreqL2mask);     
    *BA = ((pData->PulseFreqL & 0x10) << 4)  |  ((pData->PulseFreqR & 0x10) >> 4);   //     
    smprintf("\nSetTacktParam: line's 2  Freq value = 0x%x",*BA);
  }
//
// uroven stroba 0 levoi storoni liniya 1 ... stroba 3 pravoi sotony liniya 2
  BA=(USHORT*)(jj + _LevStrL0mask);     
  for (ii=0,p = (unsigned char*)&pData->Gate0LevelLn1R; ii<8; ++ii)
  {
    a = *p | (*(p+8) << 8); // skrestit lev i prav storony

#ifdef _us46_cpp_prn
    if (ii < 4)   smprintf("\nSetTacktParam: line 1, gate %d, levelr - levell  = %x", ii,a);
         else  smprintf("\nSetTacktParam: line 2, gate %d, levelr - levell  = %x", ii-4,a);
#endif

    *BA++=a;    //
    p++;
  }
//
// amplitudy zi
    BA=(USHORT*)(jj + _AMPZI1mask);
    for (ii=0,p = (unsigned char*)&pData->PulseAmpLn1R ; ii<2; ++ii)
    {
     a = ((15 - ((*p & 7) << 1))*10) | ((15 - ((*(p+2) & 7) << 1))*10 << 8); // skrestit lev i prav storony
     *BA++ = a;    //
     p++;
    }
//
// Изменяем только биты BScanSignalsCutOnParamBitNum в соответствии с командой
    BA=(USHORT*)(jj + BScanCutStartFrac1mask);
    a = *BA;
    a &=  ~((1 << BScanSignalsCutOnParamBitNum) | (1 << (BScanSignalsCutOnParamBitNum+8)));
    a |= ((pData->SwitcherLn1R & BscanSignalsCutOnMask != 0) << BScanSignalsCutOnParamBitNum) | ((pData->SwitcherLn1L & BscanSignalsCutOnMask != 0) << (BScanSignalsCutOnParamBitNum+8));
    *BA = a;
    BA=(USHORT*)(jj + BScanCutStartFrac2mask);
    a = *BA;
    a &=  ~((1 << BScanSignalsCutOnParamBitNum) | (1 << (BScanSignalsCutOnParamBitNum+8)));
    a |= ((pData->SwitcherLn2R & BscanSignalsCutOnMask != 0) << BScanSignalsCutOnParamBitNum) | ((pData->SwitcherLn2L & BscanSignalsCutOnMask != 0) << (BScanSignalsCutOnParamBitNum+8));
    *BA = a;
//
   TakeSemaphore(s_asd); // precaution: if ASD is not off
   for (ii = 0; ii < tactbitmsk+1; ++ii)
   {
     for (jj = 0; jj < 5; ++jj) ASD_PreBuf[ii][jj] = 0xFFFF;
   }
   xSemaphoreGive(s_asd);
//
#else
// DEVICE_EMULATION defined
    SetTactParameter(takt, 0, 0);
#endif
  Start_PLD_Work();
}
//-------------------------------------------------------------------
void ChangeVRU(UCHAR *p)
{
USHORT *BA,WSA,a;
UCHAR sideidx,lineidx,a1,a2;
uint i,tmp,tmp2, duration;
float quoeff;

  smprintf("\nChangeVRU enter");

  Stop_PLD_Work();
  Wr_RegPLD(a0x1300, 0x101);
//
  sideidx = getSideIndex(p);
  lineidx = getLineIndex(p);
//
#ifndef DEVICE_EMULATION
  BA = (USHORT*)(parreg_sz*(*p & tactbitmsk)+ExtRamStartAdr + _RazvCRmask);
  duration = *BA & 0xFF;
// WSA address is the same for both sides - use left side WSA value
  BA = (USHORT*)(parreg_sz*(*p & tactbitmsk)+ExtRamStartAdr + _WSALmask);
  a = *BA++;
  Lo(WSA)=(UCHAR)a;
  a=*BA;
  Hi(WSA)=(UCHAR)a;

//  simprintf("\nChangeVRU: WSA = %x",WSA);

  if (WSA<extramstart) WSA= extramstart;
//
  p++;
  tmp = *p++;           // start point delay
  a1 = *p++;            // start point level
  tmp2 = *p++;          // end point delay
  a2 = *p++;            // end point level
//
  a1 =  correctDACValue(a1);
  a2 =  correctDACValue(a2);
//

  if (tmp2 != tmp)  quoeff=(float)(a2-a1)/(tmp2-tmp);
//
  for (i=tmp; i<=tmp2 && i<duration; ++i)
  {
      BA=(USHORT*)(WSA+(((i<<2)+lineidx+1)<<1)+ramstart);

//      simprintf("\nChangeVRU: BA = %x",BA);

      a = *BA;
      if (sideidx)
      { // income data for the right side
         a &= 0xFF;
         a |= (a1+(uchar)(quoeff*(i-tmp))) << 8;
      }
       else
       {
         a &= 0xFF00;
         a |= (a1+(uchar)(quoeff*(i-tmp)));
       }
      *BA = a;
  }
#lse
#endif
  Start_PLD_Work();
//
}
//-------------------------------------------------------------------
//
void ChangeStrobs(UCHAR *p)
{
USHORT *BA,WSA,a;
UCHAR sideidx,lineidx,takt,a1,a2;
USHORT i,tmp,tmp2;
USHORT razvLen;
//
  smprintf("\nChangeStrobs enter");
  Stop_PLD_Work();
  Wr_RegPLD(a0x1300, 0x101);
//
  sideidx = getSideIndex(p);
  lineidx =  getLineIndex(p);
  takt = getTacktNumber(p);
  p++;         // *p - nomer stroba 0..
//
#ifndef DEVICE_EMULATION
// WSA address is the same for both sides - use left side WSA value
  BA = (USHORT*)(parreg_sz*takt+ExtRamStartAdr + _WSALmask);
  a = *BA++;
  Lo(WSA)=(UCHAR)a;
  a=*BA;
  Hi(WSA)=(UCHAR)a;
  if (WSA<extramstart) WSA= extramstart;
//
// uroven stroba
  if (lineidx) BA=(USHORT*)(parreg_sz*takt+ExtRamStartAdr + _LevStrR0mask);
    else  BA= (USHORT*)(parreg_sz*takt+ExtRamStartAdr + _LevStrL0mask);
  for (i=0; i<*p; ++i)  BA++;
  a = *BA;
//
  if (sideidx)
  {
    a &= 0xFF;
    if (*(p+4) == 0) a |= *(p+3) << 8; // 1echo
        else a = a | ((*(p+3) | 1) << 8); // 2echo
  }
    else
    {
      a &= 0xFF00;
      if (*(p+4) == 0) a |= *(p+3);
          else a = a | *(p+3) | 1;
    }
  *BA = a;
//
//   simplePrintf("\nChangeStrobs: takt = %d, strob = %d, addr = 0x%x, level = 0x%x ",takt, *p, (DWORD)BA,a );

  if (lineidx!=0)
  {
    a1=mask[(*p & 0x03) | 0x04];// else smesch=4
    a2=mask2[(*p & 0x03) | 0x04];
  }
      else
      {
        a1=mask[*p & 0x03];
        a2=mask2[*p & 0x03];
      }
//
  if (sideidx)
  {
   if (*(p+5) & 1) ASDtype[takt] |= (a1 << 8);
      else  ASDtype[takt] &= (a2 << 8) | 0xFF;
  }
    else
    {
       if (*(p+5)&1) ASDtype[takt] |= a1;
             else ASDtype[takt] &= a2 | 0xFF00; 
    }

  ASD_Buffer[takt] = ASDtype[takt];

  tmp =  *(p+1);  // strob start
  tmp2 = *(p+2);  // strob end
//


//      simplePrintf("\nStrob: WSA = %x   ",WSA);
//      simplePrintf("\nStrob:  start = %d   ",tmp);
//      simplePrintf("\nStrob:  end = %d   ",tmp2);

     BA=(USHORT*)(parreg_sz * takt + ExtRamStartAdr + _RazvCRmask);

     razvLen = *BA & 0xFF;

      for (i=0; i<razvLen; ++i)
      {
        BA=(USHORT*)(WSA+(((i<<2)+3)<<1)+ramstart);
        a=*BA;
        if ((i>=tmp) && (i < tmp2))
        {
            if (sideidx)  a |= (a1 << 8);
                else        a |= a1;
        }
         else
         {
           if (sideidx) a &= (a2 << 8) | 0xFF;
              else a &= a2 | 0xFF00;
         }
//
        *BA = a;
     }
#else
#endif
  Start_PLD_Work();
}
//-------------------------------------------------------------------
void Change2Tp(UCHAR *p)
{
USHORT a, tmp, *BA;
UCHAR sideidx,lineidx;
//
  smprintf("\nChange2TP enter");
//
  Stop_PLD_Work();
  Wr_RegPLD(a0x1300, 0x101);
//
  sideidx = getSideIndex(p);
  lineidx =  getLineIndex(p);
//
  tmp = *(p+1) | (*(p+2) << 8);

  if (tmp > TprizmMax)
  {
     if (tmp < TprizmLargeMax)   tmp=TprizmLargeMax-tmp;
           else tmp = 0;   
    Wr_RegPLD(a0x13A0, 0x202);
  }
     else  
     {
         tmp = TprizmMax - tmp;
         Wr_RegPLD(a0x13A0, 0);
     }

//  simplePrintf("\n side = %d, line = %d, Tprizm = %d", sideidx, lineidx, tmp);

#ifndef DEVICE_EMULATION
  if (lineidx)
  {
     BA=(USHORT*)(parreg_sz*(*p & tactbitmsk)+ExtRamStartAdr + _TprizmR1mask);
     a = *BA;
     if (sideidx)
     {
        a &= 0xFF;
        a |= tmp << 8;
     }
       else
       {
         a &= 0xFF00;
         a |= tmp & 0xFF;
       }
    *BA = a;
//
     BA=(USHORT*)(parreg_sz*(*p & tactbitmsk)+ExtRamStartAdr + _TprizmR2mask);
     a = *BA;
     if (sideidx)
     {
        a &= 0xFF;
        a |= tmp & 0xFF00;
     }
       else
       {
         a &= 0xFF00;
         a |= tmp >> 8;
       }
  }
    else
    {
       BA=(USHORT*)(parreg_sz*(*p & tactbitmsk)+ExtRamStartAdr + _TprizmL1mask);
       a = *BA;
       if (sideidx)
       {
        a &= 0xFF;
        a |= tmp << 8;
       }
       else
       {
         a &= 0xFF00;
         a |= tmp & 0xFF;
       }
      *BA = a;
//
      BA=(USHORT*)(parreg_sz*(*p & tactbitmsk)+ExtRamStartAdr + _TprizmL2mask);
      a = *BA;
      if (sideidx)
      {
        a &= 0xFF;
        a |= tmp & 0xFF00;
      }
       else
       {
         a &= 0xFF00;
         a |= tmp >> 8;
       }
    }
  *BA = a;
#else
#endif
  Start_PLD_Work();
}
//-------------------------------------------------------------------
void setBScanSignalsCutStartCmd(tBSCANSIGNALSCUTSTARTCMD * pData) 
{
unsigned short a, *BA;
unsigned char sideidx,lineidx,takt;
//
  smprintf("\nsetBScanSignalsCutStartCmd enter");
//
  Stop_PLD_Work();
  Wr_RegPLD(a0x1300, 0x101);
//
  sideidx = getSideIndex(&pData->SLT);
  lineidx =  getLineIndex(&pData->SLT);
  takt = getTacktNumber(&pData->SLT);
//
#ifndef DEVICE_EMULATION
  if (lineidx)
  {
      BA=(USHORT*)(parreg_sz* takt+ExtRamStartAdr + BScanCutStart2mask);
  } 
      else
      {
          BA=(USHORT*)(parreg_sz* takt+ExtRamStartAdr + BScanCutStart1mask);
      }
      a = *BA;
      if (sideidx)
      {
        a &= 0xFF;
        a |= pData->Start << 8;

      }
          else
          {
              a &= 0xFF00;
              a |= pData->Start;
          } 
   *BA = a;
   if (lineidx)
   {
       BA=(USHORT*)(parreg_sz* takt+ExtRamStartAdr + BScanCutStartFrac2mask);
   } 
      else
      {
          BA=(USHORT*)(parreg_sz* takt+ExtRamStartAdr + BScanCutStartFrac1mask);
      }
    a = *BA;
    if (sideidx)
      {
        a &=  0xFF | (1<< (BScanSignalsCutOnParamBitNum+8));
        a |= (pData->StartFrac & BscanSignalsCutStartMask) << 8;
      }
          else
          {
              a &= 0xFF00 | (1<< BScanSignalsCutOnParamBitNum);
              a |= pData->StartFrac & BscanSignalsCutStartMask;
          } 
   *BA = a;
#else
#endif
  Start_PLD_Work();
}

//-------------------------------------------------------------------
DWORD getAScanSumValueFromPLD(USHORT *pPLDAddress, unsigned char side)
{
unsigned char element[4];
unsigned  char ii;

    element[3] = 0; 
    for (ii =0; ii < 3; ++ii)
    {
        if (side == 0) element[ii] = (unsigned char)*pPLDAddress;
            else element[ii] = (unsigned char)(*pPLDAddress >> 8);
        pPLDAddress++;
    }    
    return ReadLE32U(element);
}
//-------------------------------------------------------------------
#ifndef AC_dis
void getAScanSumThValue(unsigned char *pDestination, unsigned char side, unsigned char line, unsigned char receiver)
{
unsigned short *thMemPtr;
unsigned char ii;

    if (line == 0)
    {
        thMemPtr = (unsigned short*)&( (*pAcThLine0)[receiver] );
    }
        else
        {
            thMemPtr = (unsigned short*)&( (*pAcThLine1)[receiver] );
        }

    for (ii=0; ii<3; ++ii)
    {
        if (side == 0)
        {
            pDestination[ii] = (unsigned char)thMemPtr[ii];
        }
            else
            {
                pDestination[ii] = (unsigned char)(thMemPtr[ii] >> 8);
            }
    }
}
#endif
//-------------------------------------------------------------------
#ifndef AC_dis
// value - значащие младшие 3 байта
void setAScanSumThValue(unsigned char side, unsigned char line, unsigned char receiver, DWORD value)
{
unsigned short *thMemPtr;
unsigned char *pByte;
unsigned char ii;

    pByte = (unsigned char*)&value;
    if (line == 0)
    {
        thMemPtr = (unsigned short*)&( (*pAcThLine0)[receiver] );
    }
        else
        {
            thMemPtr = (unsigned short*)&( (*pAcThLine1)[receiver] );
        }

    for (ii=0; ii<3; ++ii)
    {
        if (side == 0)
        {
            thMemPtr[ii] &= 0xFF00;
            thMemPtr[ii] |= pByte[ii];
        }
            else
            {
                thMemPtr[ii] &= 0xFF;
                thMemPtr[ii] |= pByte[ii] << 8;
            }
    }

//   simplePrintf("\nsetAScanSumThValue: side = %d, line = %d, receiver = %d, value = 0x%x", side, line, receiver, value);
}
#endif

#ifndef AC_dis
//-------------------------------------------------------------------
void AScanSumThTablesInitialize(void)
{
unsigned char receivers;
    for (receivers = 0; receivers < NumberOfReceivers; ++receivers)
    {
        setAScanSumThValue(0, 0, receivers, cMaxAscanSumTh);
        setAScanSumThValue(0, 1, receivers, cMaxAscanSumTh);
        setAScanSumThValue(1, 0, receivers, cMaxAscanSumTh);
        setAScanSumThValue(1, 1, receivers, cMaxAscanSumTh);
    }
}
#endif
//-------------------------------------------------------------------
// находит наиболее недозаполненный массив из элементов (*pACMaxSums),
// использумых при данной таблице тактов, и возвращает количество заполненных
// у такого элемента
#ifndef AC_dis
unsigned char findTheLessSaturation(void)
{
register unsigned char tackt;
register unsigned char line;
register unsigned char side;
unsigned char receiver;
unsigned char result;

    result = ACTHRARRAY_DEPTH;
    for (line = 0; line <= 1; ++line )
    {
        for (tackt=0; tackt < NumOfTacts; ++tackt)
        {
            for (side=0; side <= 1; ++side)
            {
                receiver = receiversInCycle[side][line][tackt];
                if (receiver != RECEIVER_UNKNOWN)
                {
                    if ((*pACMaxSums)[side][line][receiver].used < result) result = (*pACMaxSums)[side][line][receiver].used;
                }
            }
        }
   }
   return result;
}
#endif
//-------------------------------------------------------------------
// упорядочивает массив по убыванию
// т.е. элемент с индексом 0 станет максимальным
void sortDownACDataSum(DWORD *pArray, unsigned int sizeOfElement)
{
register unsigned int ii;
register BOOL fArrange;
DWORD temp;
    do
    {
        fArrange = FALSE;
        for(ii=sizeOfElement-1; ii > 0; ii--)
        {
            if (pArray[ii] > pArray[ii-1])
            {
                temp = pArray[ii];
                pArray[ii] = pArray[ii-1];
                pArray[ii-1] = temp;
                fArrange = TRUE;
            }
        }
    } while (fArrange == TRUE);
}
//-------------------------------------------------------------------
#ifndef AC_dis
DWORD addACDataSum(unsigned char side, unsigned char line, unsigned char receiver, DWORD sumValue)
{
    if (sumValue > (*pACMaxSums)[side][line][receiver].Sums[ACTHRARRAY_DEPTH])
    {
        (*pACMaxSums)[side][line][receiver].Sums[ACTHRARRAY_DEPTH] = sumValue;
        sortDownACDataSum((DWORD*)&(*pACMaxSums)[side][line][receiver].Sums[1], ACTHRARRAY_DEPTH);
    }
    return (*pACMaxSums)[side][line][receiver].Sums[1];
}
#endif
//-------------------------------------------------------------------
#ifndef AC_dis
void sortACDataSumS(void)
{
register unsigned char tackt;
register unsigned char line;
register unsigned char side;
unsigned char receiver;
        for (line = 0; line <= 1; ++line )
        {
            for (tackt=0; tackt < NumOfTacts; ++tackt)
            {
                for (side=0; side <= 1; ++side)
                {
                    receiver = receiversInCycle[side][line][tackt];
                    if (receiver != RECEIVER_UNKNOWN)
                        sortDownACDataSum((DWORD*)&(*pACMaxSums)[side][line][receiver].Sums[1], ACTHRARRAY_DEPTH);
                }
            }
       }
}
#endif
//-------------------------------------------------------------------
#ifndef AC_dis
void addACDataSums(void)
{
register unsigned char tackt;
register unsigned char line;
register unsigned char side;
unsigned char receiver;

    for (line = 0; line <= 1; ++line )
    {
        for (tackt=0; tackt < NumOfTacts; ++tackt)
        {
            for (side=0; side <= 1; ++side)
            {
                receiver = receiversInCycle[side][line][tackt];
                if (receiver != RECEIVER_UNKNOWN)
                    addACDataSum(side, line, receiver, (*pACMaxSums)[side][line][receiver].Sums[0]);
            }
        }
   }
}
#endif
//-------------------------------------------------------------------
#ifndef AC_dis
void shiftACDataSums(void)
{
register unsigned char tackt;
register unsigned char line;
register unsigned char side;
unsigned char receiver;
register unsigned char ii;
    for (line = 0; line <= 1; ++line )
    {
        for (tackt=0; tackt < NumOfTacts; ++tackt)
        {
            for (side=0; side <= 1; ++side)
            {
                receiver = receiversInCycle[side][line][tackt];
                if (receiver != RECEIVER_UNKNOWN)
                {
                    for (ii = ACTHRARRAY_DEPTH; ii > 0; ii--)
                    {
                        (*pACMaxSums)[side][line][receiver].Sums[ii] = (*pACMaxSums)[side][line][receiver].Sums[ii-1];
                        if ((*pACMaxSums)[side][line][receiver].used < ACTHRARRAY_DEPTH) (*pACMaxSums)[side][line][receiver].used++;
                    }
                }
            }
        }
   }
}
#endif
//-------------------------------------------------------------------
#ifndef AC_dis
void printACDataSums(unsigned char side, unsigned char line, unsigned char tackt)
{
unsigned int ii;
unsigned char receiver;
    simplePrintf("\nACDataSums:");
    receiver = receiversInCycle[side][line][tackt];
    if (receiver == RECEIVER_UNKNOWN)
    {
        simplePrintf(" error: unknown receiver for side = %d line = %d tackt = %d", side, line, tackt);
        return;
    }
    for (ii=0; ii<=ACTHRARRAY_DEPTH; ++ii)
    {
        simplePrintf("\n %d", (*pACMaxSums)[side][line][receiver].Sums[ii]);
    }
}
#endif
//-------------------------------------------------------------------

#define SIDE_FILTER 0
#define LINE_FILTER 0
#define TAKT_FILTER 4

#ifndef AC_dis
// выполнять, когда семафор s_ACTH занят
void calculateAndSetTh(unsigned char percent)
{
register unsigned char tackt;
register unsigned char line;
register unsigned char side;
unsigned char receiver;
unsigned char receiverR;
register unsigned char ii;
unsigned char elementL[4];
unsigned char elementR[4];
register unsigned short *pParamAddress;
DWORD taktParRegionAdr;

    smprintf("\ncalculateAndSetTh enter");
//
   for (line = 0; line <= 1; ++line )
   {
       for (tackt=0; tackt < NumOfTacts; ++tackt)
       {
           DWORD maxValue;
           for (side=0; side <= 1; ++side)
           {
               receiver = receiversInCycle[side][line][tackt];
               if (receiver != RECEIVER_UNKNOWN)
               {
                   maxValue = 0;
                   for (ii = ACTHRARRAY_DEPTH; ii > 0; ii--)
                   {
                       if ( (*pACMaxSums)[side][line][receiver].Sums[ii] > maxValue)
                           maxValue = (*pACMaxSums)[side][line][receiver].Sums[ii];
                   }

                   maxValue =  maxValue + maxValue * percent / 100 + 1;
//
/*
                   if ( (side == SIDE_FILTER) && (line == LINE_FILTER) && (tackt == TAKT_FILTER) )
                   {
                       simplePrintf("\n receiver = %d, takt = %d, side = %d, line = %d, Th = %d",receiver, tackt, side, line, maxValue);
                   }
*/
//
                  if (maxValue > cMaxAscanSumTh)
                  {
                      maxValue =  cMaxAscanSumTh;
                  }
                  setAScanSumThValue(side, line, receiver, maxValue);
               }
           }
       }
   }
//
   Stop_PLD_Work();
   Wr_RegPLD(a0x1300, 0x101);
   for (tackt=0; tackt < NumOfTacts; ++tackt)
   {
       taktParRegionAdr = parreg_sz * tackt +ExtRamStartAdr;
       for (line = 0; line <= 1; ++line )
       {
           receiver = receiversInCycle[indexLeft][line][tackt];
           receiverR = receiversInCycle[indexRight][line][tackt];
           if (line == 0) pParamAddress = (unsigned short *)(taktParRegionAdr +_AScanSumTh1mask);
               else pParamAddress = (unsigned short *)(taktParRegionAdr +_AScanSumTh2mask);
           getAScanSumThValue(elementL, indexLeft, line, receiver);
           getAScanSumThValue(elementR, indexRight, line, receiverR);


//           simplePrintf("\nreceiver = %d, takt = %d, line = %d, THL =  %d", receiver, tackt, line, (elementL[1] << 8) | elementL[0]);
//           simplePrintf("\nreceiver = %d, takt = %d, line = %d, THR =  %d", receiverR,tackt, line, (elementR[1] << 8) | elementR[0]);
//           simplePrintf("\ntakt = %d, line = %d, ADR =  0x%x", tackt, line, (DWORD)pParamAddress);


           for (ii=0; ii<3; ++ii)
           {
               *pParamAddress++ = (elementL[ii] << 8) | elementR[ii];
           }
       }    
   }
   StartPLD();
}
#endif
//-------------------------------------------------------------------
// возвращает TRUE, если идет этап вычисления порогов
#ifndef AC_dis
inline BOOL isACThCalculated(void)
{
    return !((ACCalcStage == ACCALC_WAITFORDATA) || (ACCalcStage == ACCALC_NEED_TO_START));
}
#endif
//-------------------------------------------------------------------
#ifndef AC_dis
void defAScanSumThCmd(unsigned char *p)
{
  TakeSemaphore(s_ACTH);
  smprintf("\ndetAScanSumThCmd enter");
//
  disintpld
  acThPercent = *p;
  p++;
  ACThCalcPeriodByCycle = *p * 1000;

  switch(ACCalcStage)
  {
      case ACCALC_NEED_TO_START:
      case ACCALC_WAITFORDATA:
      case ACTHCALC_COMPUTEEXEC:
      {
          ACCalcStage = ACTHCALC_CNTQUERY;
          break;
      }
      default: break;
  }
  COND_IntASD_ENABLE
  xSemaphoreGive(s_ACTH);
}
#endif
//-------------------------------------------------------------------
//
void setAScanSumThCmd(unsigned char *thValuesArray){}
//-------------------------------------------------------------------
#ifndef AC_dis
void getAScanSumThCmd(void)
{
unsigned char *pMsg;
unsigned char *pb;
unsigned char tackt;
unsigned short ii;
unsigned short word;
int startTick;

    pMsg = (unsigned char*)memalloc(sizeof(szAScanSumTh + hdrsize) );
    if  (pMsg == NULL) 
    {
        simplePrintf("\ngetAScanSumThCmd - %s",getErrorStr(ERROR_MEM_ALLOCATING));
        return;
    } 
//
    startTick = xTaskGetTickCount();
    TakeSemaphore(s_ACTH);
    disintpld
    while ((get_tickdur(startTick) < 3000) && (isACThCalculated() == TRUE) );
    {
         COND_IntASD_ENABLE
         xSemaphoreGive(s_ACTH);
         vTaskDelay(tormoz);
         TakeSemaphore(s_ACTH);
         disintpld
    }
//
    if (isACThCalculated() == TRUE)
    {
        COND_IntASD_ENABLE
        xSemaphoreGive(s_ACTH);
        smprintf("\ngetAScanSumThCmd: waiting too long. Action was abandoned");
        return;
    }
    COND_IntASD_ENABLE
//
     pb = pMsg + hdrsize;
     for (ii=0; ii<szAScanSumTh; ++ii)
     {
         *pb = 0;
         ++pb;
     }
//
// в посылке требуются пороги для тактов, а не для приемников
    fillMessageHeader((tLANMESSAGEHEADER*)pMsg,idAScanSumTh,idBUM,szAScanSumTh);
     pb = pMsg + hdrsize;
     for (tackt = 0; tackt < NumOfTacts; ++tackt)
     {
        for (ii=0; ii<3; ++ii)
        {
            word = (*pAcThLine0)[receiversInCycle[indexLeft][0][tackt]][ii] & 0xFF00 | (*pAcThLine0)[receiversInCycle[indexRight][0][tackt]][ii] & 0xFF;
            WriteLE16U(pb, word);
            pb +=2; 
        } 
        for (ii=0; ii<3; ++ii)
        {
            word = (*pAcThLine1)[receiversInCycle[indexLeft][1][tackt]][ii] & 0xFF00 | (*pAcThLine1)[receiversInCycle[indexRight][1][tackt]][ii] & 0xFF;
            WriteLE16U(pb, word);
            pb +=2; 
        }
     }
    xSemaphoreGive(s_ACTH);
//
#ifdef OLD_PACKET_HEADER 
  putmsg(pMsg,szAScanSumTh+hdrsize,NULL);
#else  
  O1Data[5]  = 0;
  putmsg(pMsg,szAScanSumTh+hdrsize,attachMessageNumber);
#endif
    memfree(pMsg);
}
#endif
//-------------------------------------------------------------------
void setExendedFormatOfDPValMessageCmd(BOOL state)
{
    DP_SEMAPHORE_ENTER
    if (state == 0) fDPCoordDataExtended = 0;
        else fDPCoordDataExtended = 1;
    DP_SEMAPHORE_LEAVE
}
//-------------------------------------------------------------------

#ifndef DEVICE_EMULATION
// обработка прерывания от ПЛИС
__irq __arm void intPLD(void) 
{
#ifndef DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
    enintT1;
#ifdef LEVEL_SENSITIVE_PLD_INT_SERVICE
    BSCAN_READY_SIGN_CLEAR
#endif
#endif
    RESET_INT_PLD
    VICADDRESS = 0;
}
#endif
//-----------------------------------------------------------------------------------------------------------
inline BOOL isACDataReady(void)
{
    return ((Rd_RegPLD(BScanASDCRegAddr) & 0x4040) == 0);
}
//-----------------------------------------------------------------------------------------------------------
// хотя бы по одной линии состояние АК изменилось
// данные биты имеют смысл, если флаги готовности данных АК установлен - isACDataReady()
inline BOOL isACDataChanged(void)
{
    return ((Rd_RegPLD(BScanASDCRegAddr) & 0x2020) != 0);
}
//-----------------------------------------------------------------------------------------------------------
#ifndef DEVICE_EMULATION
// вызывается в обработке прерывания от таймера, если не определено DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
// иначе вызов в обработке прерывания от ПЛИС
// возвращает TRUE, если требуется переключение на задачу moveLargeBScan()
// к моменту вызова задача moveLargeBScan() должна быть гарантированно в состоянии suspended и аналогично:
// пока работает задача, не должно быть реакии на прерывание, хотя даже если его сигнал имеет место
unsigned int intASD(void)
{
register BOOL BScanRoutine = FALSE;
register unsigned int toSwitchTask = FALSE; // станет TRUE, если задача, на которую
// надо переключиться, имеет такой же приоритет или выше

#ifndef DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
  disintT1
  RESET_INT_T1
#else

#ifdef LEVEL_SENSITIVE_PLD_INT_SERVICE
    BSCAN_READY_SIGN_CLEAR
#endif

#endif

  if (EnableASD)  ReadASD();

  if (EnableBScan)
  {
#ifndef AC_dis
      if (isACThCalculated() == FALSE)
      {
          if (acThCalcPeriodCnt != 0) acThCalcPeriodCnt--;
          if (acThCalcPeriodCnt == 0) ACCalcStage = ACTHCALC_CNTQUERY;
      }
#endif
      shiftValue = getShiftSensorValue(mainShiftSensorNumber);
#ifndef AC_dis
      if ( shiftValue || fMScan || (gatheringSumCnt != 0) && isACDataReady() || (ACCalcStage == ACTHCALC_CNTQUERY))
// данные АК при (gatheringSumCnt == 0) будут считываться, когда будет необходимость отправки B-развертки
#else
      if (shiftValue || fMScan)
#endif
      {
// ў® ўаҐ¬п ®ЇаҐ¤Ґ«Ґ­Ёп ¬ ЄбЁ¬ «м­®© бг¬¬л Ђ-а §ўҐавЄЁ 
// § ¤ з  ®ва Ў влў Ґв Є ¦¤л© жЁЄ«
          BScanRoutine = TRUE;
          toSwitchTask = xTaskResumeFromISR(h_moveLargeBScan);
          T3MR0change
       }
  } // EnableBScan
//

#ifdef DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
// если обработка сигнала по уровню, то этот бит ТОЖЕ должны сбрасывать, но после того, как сигнал активный уровень сигнала будет устранен
   RESET_INT_PLD
#endif

    if (BScanRoutine == FALSE) START_BSCAN_PERIOD

    VICADDRESS = 0;
    return toSwitchTask;
}
#endif
//-------------------------------------------------------------------
void ReadASD(void)
{
USHORT a, r;
register UCHAR i,j;
USHORT at,ab;
//USHORT *pasd;

   Wr_RegPLD(BScanASDCRegAddr, 0);
//
   r = BScanASD_0;
   for (i=0; i<NumOfTacts; ++i)
   {
     a = Rd_RegPLD(r);
     at=ASDtype[i];

     ab = 0;
     for (j=0; j<4; ++j)
     {
       ab |= ASD_PreBuf[i][j];
       ASD_PreBuf[i][j] =  ASD_PreBuf[i][j+1];
     }
     ab |= a;
     ASD_PreBuf[i][4] = a;
     ASD_Buffer[i] |=(a & ~at);
     ASD_Buffer[i] &=(ab | ~at);
     r++;
   }
}
//-------------------------------------------------------------------
// uses AScan buffer - tempbuf
void read_asd(void)
{
register UCHAR ii,jj;
USHORT *p;
//
  if (EnableASD)
  {
    TakeSemaphore(s_asd);
    if (get_tickdur(ASDSendTick) > TimeToTick(ASDSendTime) )
    {
     tempbuf[0] = idASD;
     tempbuf[1] = idBUM;
     jj = NumOfTacts << 1;
     tempbuf[2] =  jj;
     tempbuf[3] =  0;
//
#ifdef OLD_PACKET_HEADER
     p = (USHORT*)&tempbuf[4];
#else
     tempbuf[5] =  0;
     p = (USHORT*)&tempbuf[6];
#endif
//
     for (ii=0; ii<NumOfTacts; ++ii)
     {
         *p++ = ASD_Buffer[ii];
         ASD_Buffer[ii] = ASDtype[ii];
     }
#ifdef OLD_PACKET_HEADER
     putmsg(tempbuf,jj+hdrsize,NULL);
#else
     putmsg(tempbuf,jj+hdrsize,attachMessageNumber);
#endif
     ASDSendTick = xTaskGetTickCount();
    }
   xSemaphoreGive(s_asd);
  }
}
//-------------------------------------------------------------------
void setImitDPStep(unsigned char _imitDpStepValue, unsigned char DPNum )
{
    imitDPStep[DPNum] = _imitDpStepValue;
}
//-------------------------------------------------------------------
void fillDPMessageHdr(void *pDPMessage, BOOL extendedFormat)
{
register tLANMESSAGEHEADER *pHdr =  &((tDPCOORDMESSAGE*)pDPMessage)->Header;

/*  
  pHdr->Sourse =  idBUM; 
  pHdr->Id = idDPval;
  pHdr->Size =  szDPVal ;
#ifndef OLD_PACKET_HEADER
  pHdr->NotUse =  0;
#endif
*/
    if (extendedFormat  == FALSE)
    {
        fillMessageHeader(pHdr, idDPval, idBUM, szDPVal);
    }
        else
        {
            fillMessageHeader(pHdr, idDPvalEx, idBUM, szDPValEx);
        }
}
//-------------------------------------------------------------------
// DPNum -  абсолютный номер датчика смещения (ДП или имитатора)
void fillMessageByDPStepData(unsigned char DPNum, void *pDPMessage, unsigned char DPData, BOOL extendedFormat)
{
register  unsigned short *pBothFields; // Ї®«Ґ DPNumber ўла®ў­Ґ­® Ї® USHORT
register  unsigned char dpNumField;
   if (DPNum & IMITATORMASK)
   {
       dpNumField  = DPNum & ~IMITATORMASK | IMITSIGNMASK;
   }
       else
       {
           dpNumField  = DPNum;
       }
   if (extendedFormat == FALSE)
   {
       register tDPCOORDMESSAGE *msgPtr = (tDPCOORDMESSAGE*)pDPMessage;
         pBothFields = (USHORT *)&msgPtr->Data.DPNumber;
   }
       else
       {
          register tDPCOORDMESSAGEEX *msgPtr = (tDPCOORDMESSAGEEX*)pDPMessage;
          pBothFields = (USHORT *)&msgPtr->Data.DPNumber;
       }

  *pBothFields = (DPData << 8) |  dpNumField;

  if (DPData != 0)
  {
  volatile register unsigned int wasNegative;
    wasNegative = (unsigned int) (Pathcoord[DPNum] < 0);
    if (DPData & 0x80)
    { // negative offset
    volatile register unsigned char compliment;
        compliment = (DPData ^ 0xFF) + 1;
        Pathcoord[DPNum] -= compliment;
        if ( (!wasNegative) || (wasNegative) && (Pathcoord[DPNum] < 0) )
        {

            if (DPNum == mainShiftSensorNumber)
            {
                displayCoord += compliment;
            }
        }
            else
            {// имело место переполнение
                Pathcoord[DPNum] = MaxNegativePathCoord;
            }
    }
       else
       {
           Pathcoord[DPNum] += DPData;
           if ( (wasNegative) || (!wasNegative) && (Pathcoord[DPNum] > 0) )
           {
               if (DPNum == mainShiftSensorNumber)
               {
                   displayCoord += DPData;
               }
           }
               else
               {// имело место переполнение
                   Pathcoord[DPNum] = MaxPositivePathCoord;
               }
       }
  }
}

//-----------------------------------------------------------------------------------------------------------
#ifndef OLD_PACKET_HEADER
#define moveLargeBScanInit \
{\
    header[0] =  (idBUM<<8)| idLargeBrazv; \
    header[1] = header[2] = 0; \
    header[3] = 0; \
    fillMessageHeader(&aScanSumMsgHdr,idAScanSumAndTh,idBUM,0); \
    fillMessageHeader(&acStateMsgHdr,idAcContactState,idBUM,0); \
}
#else
#define moveLargeBScanInit \
{\
    header[0] =  (idBUM<<8)| idLargeBrazv; \
    header[1] = header[2] = 0; \
    fillMessageHeader(&aScanSumMsgHdr,idAScanSumAndTh,idBUM,0); \
    fillMessageHeader(&acStateMsgHdr,idAcContactState,idBUM,0); \
}
#endif
//-----------------------------------------------------------------------------------------------------------

#ifdef DEVICE_EMULATION
#ifdef OLD_PACKET_HEADER
USHORT header[3];
#else
USHORT header[4];
#endif
unsigned int pw;
tLANMESSAGEHEADER aScanSumMsgHdr;
tLANMESSAGEHEADER acStateMsgHdr;
#endif

inline void moveLargeBScanBody(unsigned short *pHeader)
{
volatile register DWORD line = 0;
volatile register USHORT i, j, k, numSignalOffs;
unsigned short tactMask;
UCHAR fDPMsg;        // не равно 0, если было отослано сообщение ДП(имитатора)
//
    disintpld
// прерывания от PLD не должны открыться, если произойдет переключение на другую задачу из-за
// блокировки putmsg() и т.п
    COND_INT_ASD_ENABLE_LOCK
    START_BSCAN_PERIOD
#ifndef AC_dis
      if (ACCalcStage == ACTHCALC_CNTQUERY)
      {
          Wr_RegPLD(ACControlReg, ACControlValue_OFF); // чтобы привести автомат в исходное состояние
          ACCalcStage = ACTHCALC_WAITFORDATA;
          Wr_RegPLD(ACControlReg, ACControlValue_TUNE); // чтобы запустить новый цикл с выключенным ЗИ
          Wr_RegPLD(ACSumStartReg, ACSumStartValue);
      }
          else
          {
              if ( (ACCalcStage == ACTHCALC_WAITFORDATA) && (isACDataReady()) )
              {
// поиск максимальных значений сумм для всех значений сторона-линя-такт
              register unsigned short *pLinePLDData;
              register unsigned char tackt;
              register unsigned char line;
              register unsigned char side;
                  for (line = 0; line <= 1; ++line )
                  {
               if (line == 0)
               {
                   pLinePLDData = (USHORT*)(ACLineBlockStart_1);
               }
                   else
                   {
                       pLinePLDData = (USHORT*)(ACLineBlockStart_2);
                   }
               for (tackt=0; tackt < NumOfTacts; ++tackt)
               {
                   for (side=0; side <= 1; ++side)
                   {
                         (*pACMaxSums)[side][line][receiversInCycle[side][line][tackt]].Sums[0] = getAScanSumValueFromPLD(pLinePLDData,side);
//                            simplePrintf("\n takt = %d, side = %d, line = %d, receiver = %d, curval = 0x%x", tackt, side, line, receiversInCycle[side][line][tackt], (*pACMaxSums)[side][line][receiversInCycle[side][line][tackt]].Sums[0]);
                   }
                   pLinePLDData += szPLD_ACLineBlock;
              }
                  }
                  Wr_RegPLD(ACControlReg, ACControlValue_OFF); // оставим как есть, хотя новый цикл НЕ ЗАПУСТИТСЯ
                  ACCalcStage = ACTHCALC_COMPUTEQUERY;
              }
          }
#endif
//
       line = 0;
       fDPMsg = 0;

     if (shiftValue)
     {
//             simplePrintf("\nmoveBScan: num = %d, value = %d",mainShiftSensorNumber,shiftValue);
          sendDPData(mainShiftSensorNumber,shiftValue);
          fDPMsg = 1;
//
#ifndef AC_dis
         ACStateSendIntervalCntr += shiftValue;
         if (ACStateSendIntervalCntr & 0x80)
         {
             if (ACStateSendIntervalCntr <= cACStateSendInterval_Neg)
             {
                     fACStateSendIntervalReached = TRUE;
                     ACStateSendIntervalCntr =  (ACStateSendIntervalCntr ^ 0xFF) + 1;
                     ACStateSendIntervalCntr %= cACStateSendInterval_Pos;
                     ACStateSendIntervalCntr =  (ACStateSendIntervalCntr ^ 0xFF) + 1;
             }
          }
              else
              {
                  if (ACStateSendIntervalCntr >= cACStateSendInterval_Pos)
                  {
                         fACStateSendIntervalReached = TRUE;
                         ACStateSendIntervalCntr %= cACStateSendInterval_Pos;
                  }
              }
#endif
     }
         else if (fMScan == TRUE)
                 {
                     sendDPData(mainShiftSensorNumber,0);
                     fDPMsg = 1;
#ifndef AC_dis
                     fACStateSendIntervalReached = TRUE;
#endif
                 }
//
#ifndef AC_dis
      if ( (ACCalcStage == ACCALC_WAITFORDATA) && (gatheringSumCnt == 0) && isACDataReady() )
      {
        if ((isACDataChanged() || fACStateSendIntervalReached))
        {
          if (fACStateSendIntervalReached)
          {
              fACStateSendIntervalReached = FALSE;
          }
              else
              {
                  ACStateSendIntervalCntr = 0;
              }
          switch(enableACMessage)
          {
            case ACRAWDATA:
            {
                 register USHORT *pLine1Data;
                 register USHORT *pLine2Data;
                 register USHORT size;
                 USHORT thBlock[3];
                 unsigned char ii;

                     pLine1Data = (USHORT*)(ACLineBlockStart_1);
                     pLine2Data = (USHORT*)(ACLineBlockStart_2);
                     size = (aScanSumBlock_sz + aScanThBlock_sz ) *  NumOfTacts;
                     get_Access(size + sizeof(tLANMESSAGEHEADER));
                     ATTACH_MESSAGE_NUMBER_TO_HEADER(&aScanSumMsgHdr);
                     attachMessageLengthToHeader(&aScanSumMsgHdr,size);
                     put_DataByWord((USHORT*)&aScanSumMsgHdr,sizeof(tLANMESSAGEHEADER));
                     for (k=0;  k<NumOfTacts; ++k)
                     {
                         put_DataByWord(pLine1Data,aScanSumLineBlock_sz);
                         put_DataByWord(pLine2Data,aScanSumLineBlock_sz);
                         pLine1Data += szPLD_ACLineBlock;
                         pLine2Data += szPLD_ACLineBlock;
// в посылке требуются пороги для тактов, а не для приемников
                         for (ii=0; ii<3; ++ii)
                         {
                             thBlock[ii] = (*pAcThLine0)[receiversInCycle[indexLeft][0][k]][ii] & 0xFF00 | (*pAcThLine0)[receiversInCycle[indexRight][0][k]][ii] & 0xFF;
                         }
                         put_DataByWord((unsigned short*)thBlock, aScanThLineBlock_sz);
                         for (ii=0; ii<3; ++ii)
                         {
                             thBlock[ii] = (*pAcThLine1)[receiversInCycle[indexLeft][1][k]][ii] & 0xFF00 | (*pAcThLine1)[receiversInCycle[indexRight][1][k]][ii] & 0xFF;
                         }
                         put_DataByWord((unsigned short*)thBlock, aScanThLineBlock_sz);
                     }
                     release_Buffer;
            }
           case ACSTATE:
           {
               register USHORT *pLine1Data;
               register USHORT *pLine2Data;
               register USHORT aw;

                     pLine1Data = (USHORT*)(ACStateStartLine_1);
                     pLine2Data = (USHORT*)(ACStateStartLine_2);
                     aw = szAcContactStateBlock *  NumOfTacts;
                     get_Access(aw + sizeof(tLANMESSAGEHEADER));
                     ATTACH_MESSAGE_NUMBER_TO_HEADER(&acStateMsgHdr);
                     attachMessageLengthToHeader(&acStateMsgHdr,aw);
                     put_DataByWord((USHORT*)&acStateMsgHdr,sizeof(tLANMESSAGEHEADER));
                     for (k=0;  k<NumOfTacts; ++k)
                     {
                         aw = *pLine1Data;
                         aw |= *pLine2Data << 4;
                         put_DataByWord(&aw,2);
                         pLine1Data += szPLD_ACLineBlock;
                         pLine2Data += szPLD_ACLineBlock;
                     }
                     release_Buffer;
               break;
            }
         }
        }
        Wr_RegPLD(ACSumStartReg, ACSumStartValue);
        ACCalcStage = ACCALC_NEED_TO_START;
      }
#endif
//
  if (fDPMsg)
  {
// когда идет сбор сумм, эта задача возобновляется в т.ч тогда, когда нет повода для B(М)-развертки
// поэтому исполняем код только, если fDPMsg
      while(line < 2)
      {
         volatile register unsigned short NumOfSignals;
         if (line == 0) Wr_RegPLD(BScanASDCRegAddr, 0x202); // «Ё­Ёп 0
                 else  Wr_RegPLD(BScanASDCRegAddr, 0x303);
//
         for (k=0, tactMask = 0; k<NumOfTacts; ++k)
         {
//            NumOfSignals = *(USHORT*)(BScanASD_0+(k<<7));
              NumOfSignals = Rd_RegPLD(BScanASD_0 + (k<<7));

//               simplePrintf("\n line = %d, takt = %d,signals = 0x%x",line,k,NumOfSignals);
//
            if (NumOfSignals != 0)
            {
                if ((NumOfSignals & 0xFF00) > 0x800)
                {
                   NumOfSignals &= 0xFF;
                   NumOfSignals |= 0x800;
                }
                if ((NumOfSignals & 0xFF) > 0x8)
                {
                  NumOfSignals &= 0xFF00;
                  NumOfSignals |= 0x8;
                }
                j = MAX(NumOfSignals >> 8, NumOfSignals & 0xFF);

                register USHORT a  = j * bScanSignalBlock_sz +2;

                get_Access(a+hdrsize);
#ifdef OLD_PACKET_HEADER
              pHeader[1] = a;
              pHeader[2] =  ((NumOfSignals & 0xF00) << 4) | ((NumOfSignals & 0xF) << 8)  | (line<<6) | k;
#else
              pHeader[1] = a;
              ATTACH_MESSAGE_NUMBER(*(UCHAR*)&pHeader[2])
              pHeader[3] =  ((NumOfSignals & 0xF00) << 4) | ((NumOfSignals & 0xF) << 8)  | (line<<6) | k;
#endif

              put_DataByWord(pHeader, sizeof(header));
              numSignalOffs = 0;
              for (i=0; i<j; ++i)
              {
#ifndef DEVICE_EMULATION
                 pw = (USHORT*)(BScanASD_1 + tactMask + numSignalOffs);
//                 put_DataByWord(pw,bScanSignalBlock_sz);
#else
//                 pw = BScanASD_0 + tactMask + numSignalOffs;
                 pw = BScanASD_1 + tactMask + numSignalOffs;
//                 put_DataByWord(pw,bScanSignalBlock_sz + 2);
#endif
                 put_DataByWord(pw,bScanSignalBlock_sz);
                 numSignalOffs += (bScanSignalBlock_sz+2);
              }
              release_Buffer;
          }
          tactMask += cTactMaskStep;
         } // for
//
          line++;
      } // while
  }
  if ((fDPMsg) || (fMScan)) initMScanFlags();


/*
#ifndef DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION
         if (INT_T1_PENDING)
#else
         if ( PLD_INT_PIN_STATE == 0)
#endif
         {

         }
*/
        COND_INT_ASD_ENABLE_UNLOCK

        SUSPEND_ISR_TASK();
}
//-----------------------------------------------------------------------------------------------------------

void moveLargeBScan(void *ppar)
{ 

#ifndef DEVICE_EMULATION
#ifdef OLD_PACKET_HEADER
USHORT header[3];
#else
USHORT header[4];
#endif
USHORT *pw; 
tLANMESSAGEHEADER aScanSumMsgHdr;
tLANMESSAGEHEADER acStateMsgHdr;
#endif

#ifndef DEVICE_EMULATION

  moveLargeBScanInit
  vTaskSuspend(NULL);
  while(1)
  {
      moveLargeBScanBody(header);
  }
#else
{
    if ((fMScan) || (shiftValue))
    {
        moveLargeBScanBody(header);
    }
}
#endif
}
//-----------------------------------------------------------------------------------------------------------
void initMScanFlags(void)
{
      fResetMScanTimer = 1; // ¤«п ЇҐаҐ§ ЇгбЄ  в ©¬Ґа 
       fMScan = 0;
}
//-----------------------------------------------------------------------------------------------------------
void mScanAlarm(void)
{
    if(fResetMScanTimer) 
    {
        MScanSendTick = xTaskGetTickCount();
        fResetMScanTimer = 0;
    }
        else 
            if  ((!fMScan) && (get_tickdur(MScanSendTick) > TimeToTick(MScanSendTime) ))
            {
                     fMScan = 1;
            }
}
//-----------------------------------------------------------------------------------------------------------
void setRelayCtrlSignals(unsigned char command )
{
    switch(command)
    {
        case setKCmd:
        case resetKCmd:
           Wr_RegPLD(a0x1307, command);
           vTaskDelay(TimeToTick(cRelayCtrlSignalDur));
           Wr_RegPLD(a0x1307, KSignalsOff);
            break;
        default: 
           Wr_RegPLD(a0x1307, KSignalsOff);
    }
}
//-----------------------------------------------------------------------------------------------------------
void setDPOption(unsigned short value)
{
    Wr_RegPLD(DPCONTROLREGADR , value);
}
//-----------------------------------------------------------------------------------------------------------
void setTrolleyDP()
{
    if (lengthPathEncoderDividerOffPar == 1) setDPOption(0);
        else setDPOption(SETDIVIDER3BIT);
    fIsTrolleyDP = TRUE;
}
//-----------------------------------------------------------------------------------------------------------
void setScanerDP()
{
    setDPOption(SETSCANERDPMAJORBIT);
    fIsTrolleyDP = FALSE;
}
//-----------------------------------------------------------------------------------------------------------
void KSwitch(UCHAR fScanerOn)
{
  if (fScanerOn)
  { 
      setScanerDP();   // б­ з «  ЇҐаҐЄ«озЁ¬ ўе®¤л „Џ
      setRelayCtrlSignals(setKCmd); //   в®«мЄ® Ї®в®¬ Ўг¤Ґ¬ ¤ҐаЈ вм аҐ«Ґ, ЇаЁзҐ¬ нв  § ¤ з  Ўг¤Ґв ЇаЁ®бв ­®ў«Ґ­ 
  }
     else 
     {
         setTrolleyDP(); // б­ з «  ЇҐаҐЄ«озЁ¬ ўе®¤л „Џ
         setRelayCtrlSignals(resetKCmd);
     }
  REDEFINE_DP_CYCLE_PROC
}
//-----------------------------------------------------------------------------------------------------------
void scanerSwitch(UCHAR *p)
{
  smprintf("\nscanerSwitch: ");
  if (*p & 1)
  { // Ї®¤Є«озЁвм бЄ ­Ґа
      smprintf("switch to scaner Av17");
      KSwitch(setScanerOn); 
  }
     else 
     { // Є ­ «л бЇ«®и­®Ј® Є®­ва®«п
         smprintf("switch to trolley's channels");
         KSwitch(setScanerOff); 
     }
}
//-----------------------------------------------------------------------------------------------------------
void ACThCalcInit(void)
{
#ifndef AC_dis
    gatheringSumCnt = cGATHERINGSUMDURATION;
    acThCalcPeriodCnt = 0; // на всяк случай
    ACCalcStage = ACCALC_NEED_TO_START;
    ACThCalcPeriodByCycle = cACThCalcPeriodByCycle;
#endif
    Wr_RegPLD(ACControlReg, ACControlValue_OFF);
}




