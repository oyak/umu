// include-file 
// исходный _us4618.cpp
// изменена varsinit(),   SetDPImit()
//  _us4618.cpp
// исправлена ошибка в ustsk()
//
//  _us4617.cpp
// изменена SendVersionInfo()
// 
// _us4616.cpp
// исправлена ошибка
//  _us4615.cpp - ?
// исправлена ошибка в SendVersionInfo()
//
// _us4613.cpp
// изменены ASD_Off(), parcer(), lanmsgparcer()
//
//  _us4612.cpp
// изменена SendVersionInfo()


//  _us461.cpp
// новый формат заголовка - 6 байт - если не определено OLD_PACKET_HEADER
//_us461.cpp
// добавлена условная компиляция LARGE_BSCAN


//-------------------------------------------------------------------------------------------------------------------
// dataLength - длина данных в pmsg, значение не меньше hdrsize
// возвращает:
// положительное число - длина обработанной команды вместе с заголовком
// иначе:
// -1 - неизвестный код команды
// -2 - команда принята не целиком

static tres parcer(UCHAR* pmsg,USHORT dataLength)
{
tres res;

     switch ( ReadLE32U(pmsg) & 0xFFFF00FF)
     {
        case Gc(idNtact,szNtact):
        {
             res =  szNtact+hdrsize;
             if (dataLength < res) res = -2;
                 else SetNumOfTacts(pmsg+hdrsize);
             break;
        }
//
        case Gc(idTactpar,szTactpar):
        { 
             res =  szTactpar+hdrsize;
             if (dataLength < res) res = -2;
                 else SetTacktParam((tTACTPARAMSCMD*)(pmsg+hdrsize));
             break; 
        }
//
        case Gc(idVRU,szVRU):
        {
             res =  szVRU+hdrsize;
             if (dataLength < res) res = -2;
                 else ChangeVRU(pmsg+hdrsize);
             break;
         }
//
        case Gc(idStrobpar,szStrobpar):
        {  
             res =  szStrobpar+hdrsize;
             if (dataLength < res) res = -2;
                 else ChangeStrobs(pmsg+hdrsize);
             break;
        }
//
        case Gc(id2Tp,sz2Tp):
        {
             res =  sz2Tp+hdrsize;
             if (dataLength < res) res = -2;
                 else Change2Tp(pmsg+hdrsize);
             break;
         } 
//
        case Gc(idOnAscan,szOnAscan):
        {
             res =  szOnAscan+hdrsize;
             if (dataLength < res) res = -2;
                 else StartAScan(pmsg+hdrsize);
             break;
        }
//
        case Gc(idBUMctrl,szBUMctrl):
        {
             res =  szBUMctrl+hdrsize;
             if (dataLength < res) res = -2;
                 else BUMctrlproc(pmsg+hdrsize);
             break;
        }
//
        case Gc(idDPset,szDPset):
        {
             res =  szDPset+hdrsize;
             if (dataLength < res) res = -2;
                 else SetDPval(pmsg+hdrsize);
             break;
        } 
//
        case Gc(idDPim,szDPim):
        { 
             res =  szDPim+hdrsize;
             if (dataLength < res) res = -2;
                 else setDPImitCmd(pmsg+hdrsize);
             break;
        }
//
        case Gc(idKSwitch,szKSwitch):
        { 
             res =  szKSwitch+hdrsize;
             if (dataLength < res) res = -2;
                 else scanerSwitch(pmsg+hdrsize);
             break;
        }
//
        case Gc(idDPimEx,szDPimEx):
        { 
             res =  szDPimEx+hdrsize;
             if (dataLength < res) res = -2;
                 else setDPImitExCmd(pmsg+hdrsize); 
             break;
        }
//
        case Gc(idDPScanImit,szDPScanImit):
        { 
             res =  szDPScanImit+hdrsize;
             if (dataLength < res) res = -2;
                 else setScanImitCmd(pmsg+hdrsize); 
             break;
        }
//
        case Gc(idBScanSignalsCutStart,szBScanSignalsCutStart):
        { 
             res =  szBScanSignalsCutStart+hdrsize;
             if (dataLength < res) res = -2;
                 else setBScanSignalsCutStartCmd((tBSCANSIGNALSCUTSTARTCMD*)(pmsg+hdrsize)); 
             break;
        }
//
        case Gc(idDefAScanSumTh,szDefAScanSumTh):
        { 
             res =  szDefAScanSumTh+hdrsize;
             if (dataLength < res) res = -2;
#ifndef AC_dis
                 else defAScanSumThCmd(pmsg+hdrsize); 
#endif
             break;
        }
//
/*
        case Gc(idSetAScanSumTh,szSetAScanSumTh):
        { 
             res =  szSetAScanSumTh+hdrsize;
             if (dataLength < res) res = -2;
                 else setAScanSumThCmd(pmsg+hdrsize); 
             break;
        }
*/
//
        default:
        res = -1;
        simplePrintf("\n us46_parcer: unknown COP: ");
//
//ifdef _us46_cpp_prn
        {
          UCHAR ii,*p;

          for (ii = 0, p = pmsg; ii< 8;ii++,p++) smprintf(" %x", *p);
        }
//endif
     }
     return res;
}
//--------------------------------------------------------------------
//
unsigned char lanmsgparcer(UCHAR* buf, USHORT lng)
{
register tres res;
register int l;
register UCHAR *p = buf;
register unsigned char receiveStartOffset;
//
  l = lng;
  while (l > hdrsize)
  {
   res = parcer(p,l);
   if (res > 0)
   {
     p += res;
     l -= res;
   }
     else
     {
       if (res == -1)
       {
           p++;
           l--;
       }
           else break;   
     }
  }
  if ((l != 0) || (res == -2))
  {
    int ii;
    if (res == -2) smprintf("\nlanmsgparcer: parser returned an error -2");

    smprintf("\nlanmsgparcer: income data has the unparced %d bytes:\n",l);

    if (l > cReceiveStartOffsetMax) simplePrintf("\nlanmsgparcer: too many unparsed bytes - %d",l); // очень вероятно, что при приеме следущей порции
// данных часть их будет утрачена - см. buvrcvtsk()
//
    receiveStartOffset  = (UCHAR)l;  
    for (ii = 0; ii<l; ii++)  
    {
      buf[ii] = p[ii];
      smprintf(" 0x%x", p[ii]);
    } 
  } 
     else receiveStartOffset = 0;
  return receiveStartOffset;
}
//----------------------------------------------------------------------

void varsinit(void)
{
register UCHAR ii;
//
   NumOfTacts=1;
   fIntPLDMustBeEnable = 0;
   AscanSendTick[0] =  AscanSendTick[1] = \
   ASDSendTick = MScanSendTick = xTaskGetTickCount();

   for (ii=0; ii<4; ii++) ascan[ii] = 0;
//
   EnableASD = 0;
//
   for (ii=0; ii<tactbitmsk+1; ii++)
   {
     ASDtype[ii] = 0;
   }
   EnableBScan = 0;
//
#ifndef AC_dis
   enableACMessage = ACDISABLED;
#endif
//
#ifdef LARGE_BSCAN
  shiftValue = 0;
#ifndef AC_dis
  ACStateSendIntervalCntr = 0;
  fACStateSendIntervalReached = FALSE;
#endif
  initMScanFlags(); 
#endif

#ifndef OLD_PACKET_HEADER
    messageNumber = 0;
#endif

   DP_SEMAPHORE_ENTER
   setDPOption(STOP_DP_MACHINE_BIT);
   vTaskDelay(1);
   mainShiftSensorNumber = 0;
   for (ii=0; ii<QDP;ii++) pShiftSensorProc[ii] = NULL;

   for (ii=0; ii<QDP-1;ii++) 
   {
       setImitDPStep(DPSTEPFORWARD,ii);
       imitDPIncTime[ii] = minImitDPIncTime;
       imitDPIncTick[ii] = xTaskGetTickCount();
      doWhenImitSwitchOFF(ii);   

   }
   for (ii=0; ii<QDP*2; ii++) Pathcoord[ii] = 0;
   lDiff = 0;
   DP_SEMAPHORE_LEAVE
//
   KSwitch(setScanerOff);   

#ifndef AC_dis
   pACMaxSums = NULL;
#endif

   ACThCalcInit();
}
//-------------------------------------------------------------------
// ptr points to message body
//
void BUMctrlproc(UCHAR *ptr)
{
// smprintf("\nBUMctrlproc enter");
   switch(ReadLE16U(ptr))
   {
     case 0x43:
          PLDEn();
          break;
     case 0x44:
          PDLDis();
          break;
     case 0x46:
          StopAScan();
          break;
     case 0x47:
          StartBScan();
          break;
     case 0x48:
          StopBScan();
          break;
     case 0x4B:
          ASD_On();
          break;
     case 0x4C:
          ASD_Off();
          break;
     case 0xC2:
          Get_Uakk();
          break;
     case 0xC3:
          make_C3();
          break;
//
     case 0xDE:
          SendVersionInfo() ;
          versionInfoSendTick =  xTaskGetTickCount();
          break;
//
     case 0xF0:
          ReloadBUM();
          break;
//
     case 0x35:
         setExendedFormatOfDPValMessageCmd(TRUE);
         break;
//
     case 0x36:
         setExendedFormatOfDPValMessageCmd(FALSE);
         break;
#ifndef AC_dis
     case 0x37:
          ascanSumOn();
           break;
     case 0x38:
          acMessageOff();
          break;
     case 0x39:
          acStateOn();
           break;
     case 0x88: 
         getAScanSumThCmd();
         break; 
#endif
   }
}
//------------------------------------------------------------
void ASD_On(void)
{
  if (EnableBScan) 
  {
      TakeSemaphore(s_asd); // precaution: if we are reenabling ASD
      EnableASD = 1;
      ASDSendTick = xTaskGetTickCount();
      xSemaphoreGive(s_asd);
  }
}
//-------------------------------------------------------------------
void ASD_Off(void)
{
  EnableASD = 0;
}
//-------------------------------------------------------------------
#ifndef AC_dis
void ascanSumOn(void)
{
  if ((EnableBScan) && (enableACMessage == ACDISABLED)) enableACMessage = ACRAWDATA;
}
//-------------------------------------------------------------------
void acMessageOff(void)
{
    enableACMessage = ACDISABLED;
}
//-------------------------------------------------------------------
void acStateOn(void)
{
  if ((EnableBScan) && (enableACMessage == ACDISABLED)) enableACMessage = ACSTATE;
}
#endif
//-------------------------------------------------------------------

void make_C3(void)
{
  O1Data[0] = idTime;
  O1Data[1] = idBUM;
  O1Data[2]  = szTime;
  O1Data[3]  = 0;
//
  WriteLE32U(&O1Data[hdrsize],xTaskGetTickCount() >> 1); // для кванта 500 мкС
#ifdef OLD_PACKET_HEADER 
  putmsg(O1Data,szTime+hdrsize,NULL);
#else  
  O1Data[5]  = 0;
  putmsg(O1Data,szTime+hdrsize,attachMessageNumber);
#endif  

}

//-------------------------------------------------------------------
void SendVersionInfo(void)
{
register USHORT devicenumber;
//  fill_sftversion
    O1Data[0]= idSftw;
    O1Data[1]= idBUM;
    O1Data[2] = szSftw;
    O1Data[3] = 0;
//
    O1Data[hdrsize] = (version_number) & 0xFF;
    O1Data[hdrsize+1] = (version_number >> 8) & 0xFF;
    O1Data[hdrsize+2] = (version_number >> 16) & 0xFF;
    O1Data[hdrsize+3] = PLDVerId  & 0xFF;
    O1Data[hdrsize+4] = (PLDVerId >> 8) & 0xFF;
    O1Data[hdrsize+5] = (PLDVerId >> 16) & 0xFF;
    O1Data[hdrsize+6] = PLDVerId >> 24;
    O1Data[hdrsize+7] = 0;
#ifdef OLD_PACKET_HEADER
    putmsg(O1Data,szSftw+hdrsize,NULL);
#else
  O1Data[5]  = 0;
    putmsg(O1Data,szSftw+hdrsize,attachMessageNumber);
#endif

//
//  fill_serialnumber
    O1Data[0]= idDevnum;
    O1Data[1]= idBUM;
    O1Data[2]= szDevnum;
    O1Data[3]= 0;
    devicenumber = get_devicenumber();
    if (devicenumber != 0) WriteLE16U(&O1Data[hdrsize],devicenumber);
      else  WriteLE16U(&O1Data[hdrsize],0xFFFF); // not to be zero

#ifdef OLD_PACKET_HEADER
  putmsg(O1Data,szDevnum+hdrsize, NULL);
#else
  putmsg(O1Data,szDevnum+hdrsize, attachMessageNumber);
#endif
}

//-------------------------------------------------------------------
// !
// ptr points to message body
// see lim1
void StartAScan(UCHAR *ptr)
{
register UCHAR idx;

  TakeSemaphore(s_ascan);
//
#ifdef ONE_CHANNEL_ASCAN_ONLY
  for (idx = 0; idx < 4; ++idx)
  {// отключить ранее включенное
      ascan[idx] = 0;
  }
#endif
//
  idx =  (ptr[slt_bidx] & (linebitmsk | sidebitmsk)) >> 6;
  ascan[idx]=1;
  curchan[idx]=ptr[slt_bidx] & tactbitmsk;
  ascanstart[idx]=ptr[slt_bidx+1];
  ascanscale[idx]=ptr[slt_bidx+2];
  ascanregim[idx]=ptr[slt_bidx+3];
  ascanstrobformax[idx]=ptr[slt_bidx+4] & 3;
  if (get_tickdur(AscanSendTick[idx & 1]) > TimeToTick(AscanSendTime))
       AscanSendTick[idx & 1] = xTaskGetTickCount();


  ascancounter = 0;    

//  tx_count = 0; 
//  tx1_count = 0; 
  
  
  xSemaphoreGive(s_ascan);
//
  smprintf("\nStartAScan finished - idx=%d ascan[] =%x",idx,ascan[idx]);

}

//-------------------------------------------------------------------
// установка значения датчика пути или имитатора 
void SetDPval(UCHAR *p)
{
register unsigned char sensorNum;
register unsigned char mem;
//
  smprintf("\nSetDPval enter: ");

    sensorNum = *p & SHIFTSENSORNUMBERMASK;
    mem = *p;
    if (*p & IMITSIGNMASK)
    { 
             sensorNum |=  IMITATORMASK;
    }

    if (*p++ & MAINSENSORSIGNMASK) 
    {
        DP_SEMAPHORE_ENTER
        disintpld;
        mainShiftSensorNumber = sensorNum;
        displayCoord = 0;
        COND_IntASD_ENABLE
        redefineDPCycleProc();
        DP_SEMAPHORE_LEAVE
        smprintf("\nSetDPval: DP number = %d became main one ",sensorNum);
    }
        else
        {
            DP_SEMAPHORE_ENTER
            disintpld;
            Pathcoord[sensorNum] = ReadLE32U(p);
            if (mainShiftSensorNumber == sensorNum)
            {
                displayCoord = 0;
                mainShiftSensorPathcoordMemorized = Pathcoord[mainShiftSensorNumber];
                speedTimer = xTaskGetTickCount();
            }
            COND_IntASD_ENABLE
            WriteLE32U(&O1Data[hdrsize+1], sensorNum);
            DP_SEMAPHORE_LEAVE
            smprintf("\nSetDPval: DP number = %d. Path coordinate %d was set",sensorNum,Pathcoord[sensorNum]);
//
            O1Data[0] = idDPset;
            O1Data[1] = idBUM;
            O1Data[2]  = szDPValAck;
            O1Data[3]  = 0;
//
           O1Data[hdrsize] = mem;
#ifdef OLD_PACKET_HEADER 
            putmsg(O1Data,szDPValAck+hdrsize,NULL);
#else  
            O1Data[5]  = 0;
            putmsg(O1Data,szDPValAck+hdrsize,attachMessageNumber);
#endif  
         }
}
//-------------------------------------------------------------------
void setDPImitCmd(UCHAR *p)
{
  smprintf("\nsetDPImitCmd enter: ");
  setImit(p,FALSE);
}
//-------------------------------------------------------------------
// д.б. пройден семафор     DP_SEMAPHORE_xxx
void doWhenImitSwitchOFF(unsigned char relativeNum)
{
register unsigned char ii;

    ImitDPOn[relativeNum] = IMITDPOFF;
    redefineDPCycleProc();
// чтение ДП ПЛИС, чтобы удалить данные, т.к. колесо уже могли покрутить
// читаем 3 раза, чтобы очистить счетчик, хотя достаточно и двух
    for (ii = 0; ii < 3; ii++)
    {
        getDPShift(relativeNum);
        vTaskDelay(1);
    }
}
//-------------------------------------------------------------------
// д.б. пройден семафор DP_SEMAPHORE_xxx
void doWhenScanerImitSwitchOFF(void)
{
    doWhenImitSwitchOFF(SCANERHEIGHTSENSORNUMBER);
    doWhenImitSwitchOFF(SCANERLENGTHSENSORNUMBER);
}
//-------------------------------------------------------------------
void setImit(unsigned char *p,unsigned char fExtendedCmd)
{
register unsigned char relativeNum;
//gister int res;
tDPIMITCMDEX *pData;

  smprintf("\nsetImit enter: ");
  pData = (tDPIMITCMDEX*)p;
 
  relativeNum = pData->Main.ImitNumber & SHIFTSENSORNUMBERMASK; // маска - на всякий случай
  if ((relativeNum == SCANERHEIGHTSENSORNUMBER) && (ImitDPOn[SCANERHEIGHTSENSORNUMBER] == IMITDPSPECMODE1))
  { // невозможно включить/отключить такой командой имитатор когда уже включен имитатор сканера
      smprintf("error: imitator DP(%d) command can not be used i.e. scaner imitator is already active",pData->Main.ImitNumber);
      return;
  }
  if ((relativeNum == SCANERLENGTHSENSORNUMBER)&&(ImitDPOn[SCANERHEIGHTSENSORNUMBER] == IMITDPSPECMODE1))
  { // невозможно включить/отключить такой командой имитатор когда уже включен имитатор сканера
      smprintf("error: imitator DP(%d) is main and can not be switched on or off  i.e. scaner imitator s already active",pData->Main.ImitNumber);
      return;
  }
//
    DP_SEMAPHORE_ENTER

#ifndef DEVICE_EMULATION
#ifndef us46emu
  T1TCR = 0;  // to avoid FIQ by this way
#endif
#endif

    switch(pData->Main.State)
    {
        case 0:
        {
            disintpld // т.к. в прерывании семафор не поставишь
            doWhenImitSwitchOFF(relativeNum);
            COND_IntASD_ENABLE
            break;
        }
        case 1:
        {
        unsigned short interval;
            interval = minImitDPIncTime;
            if (fExtendedCmd) 
            {
                unsigned short intervalValue;
                intervalValue = ReadLE16U((UCHAR*)pData->EventInterval); // поле м.б. не выравнено по word
                if  (interval < intervalValue) interval = intervalValue;
            }
            imitDPIncTime[relativeNum] = interval;
            ImitDPOn[relativeNum] = IMITDPWAIT;

            redefineDPCycleProc();

#ifdef LARGE_BSCAN
           shiftValue = 0;
#endif         
          break;
         }
    }
//
#ifndef DEVICE_EMULATION
#ifndef us46emu
  T1TCR = 1;  //
#endif
#endif

    DP_SEMAPHORE_LEAVE
    smprintf("ImitDP(%d) new state = %d",relativeNum, pData->Main.State);

}
//-------------------------------------------------------------------
void setDPImitExCmd(UCHAR *p)
{
  smprintf("\nsetDPImitExCmd enter: ");
  setImit(p,TRUE);
}
//-------------------------------------------------------------------
void setScanImitCmd(UCHAR *p)
{
tDPSCANIMITCMD *pData;

    pData = (tDPSCANIMITCMD *)p;
    smprintf("\nsetScanImitCmd enter: ");
 
    if (pData->AParameter == 0)
    {
        smprintf("error: parameter A equals 0");
        return;
    }
    if (pData->BParameter == 0)
    {
        smprintf("error: parameter B equals 0");
        return;
    }
//
    if ((ImitDPOn[SCANERHEIGHTSENSORNUMBER] != IMITDPOFF) &&  (ImitDPOn[SCANERHEIGHTSENSORNUMBER] != IMITDPSPECMODE1))
    {
        smprintf("error: scaner imitator command can not be used i.e. imitator DP(%d) is active",SCANERHEIGHTSENSORNUMBER);
        return;
    }
//
    if ((ImitDPOn[SCANERHEIGHTSENSORNUMBER] != IMITDPSPECMODE1) && (ImitDPOn[SCANERLENGTHSENSORNUMBER] != IMITDPOFF)) 
    {
        smprintf("error: scaner imitator command can not be used i.e. imitator DP(%d) is active",SCANERLENGTHSENSORNUMBER);
        return;
     }
    DP_SEMAPHORE_ENTER

    mainShiftSensorNumber |=  IMITATORMASK | SCANERLENGTHSENSORNUMBER; 

#ifndef DEVICE_EMULATION
#ifndef us46emu
  T1TCR = 0;  // to avoid FIQ by this way
#endif
#endif


    switch(pData->State)
    {
        case 0:
            doWhenScanerImitSwitchOFF();
            break;
//
        case 1:
        {
        unsigned short interval;  
            interval = ReadLE16U((unsigned char*)&pData->EventInterval); // поле м.б. не выравнено по word
            if  (minImitDPIncTime > interval)  interval = minImitDPIncTime;

            mainShiftSensorNumber |=  IMITATORMASK | SCANERLENGTHSENSORNUMBER; 
            imitDPIncTime[mainShiftSensorNumber & SHIFTSENSORNUMBERMASK] = interval;
            ImitDPOn[mainShiftSensorNumber & SHIFTSENSORNUMBERMASK] = IMITDPWAIT;
            ImitDPOn[SCANERHEIGHTSENSORNUMBER] = IMITDPSPECMODE1;

            parameterA = pData->AParameter;     
            parameterB = pData->BParameter;     
            scanerImitStage =  SCANIMITINIT;
  
            pShiftSensorProc[SCANERHEIGHTSENSORNUMBER] = NULL;   // на всяк случай
            pShiftSensorProc[SCANERLENGTHSENSORNUMBER] = scanerImitProc; 
            break;
        }
    }

#ifndef DEVICE_EMULATION
#ifndef us46emu
  T1TCR = 1;  //
#endif
#endif

    DP_SEMAPHORE_LEAVE
}

//-------------------------------------------------------------------
// side - SideLeft/SideRight
// 
void make_jointSensor(UCHAR side,BOOL newState)
{
  O1Data[0] = idJAndASensor;
  O1Data[1] = idBUM;
  O1Data[2]  = szJAndASensor;
  O1Data[3]  = 0;
//

#ifdef OLD_PACKET_HEADER 
  O1Data[4]  = side;
  O1Data[5]  = jointSensorId;
  if (newState)  O1Data[6]  = cJointSensorJump;
      else O1Data[6]  = cJointSensorFall;
  putmsg(O1Data,szJAndASensor+hdrsize,NULL);
#else  
  O1Data[5]  = 0;
  O1Data[6]  = side;
  O1Data[7]  = jointSensorId;
  if (newState)  O1Data[8]  = cJointSensorJump;
      else O1Data[8]  = cJointSensorFall;

  putmsg(O1Data,szJAndASensor+hdrsize,attachMessageNumber);
#endif  

}

//-------------------------------------------------------------------
// д.б. пройден семафор DP_SEMAPHORE_xxx
// imitNumber - относительный
void imitWriteDownCycleProc(UCHAR imitNumber)
{
    disintpld
    if  (get_tickdur(imitDPIncTick[imitNumber]) > (TimeToTick(imitDPIncTime[imitNumber])) ) 
    {
        imitDPIncTick[imitNumber] = xTaskGetTickCount();
        if (ImitDPOn[imitNumber] > IMITDPOFF) 
        {
            if (ImitDPOn[imitNumber] <  IMITDPSENDL) 
            {
                ImitDPOn[imitNumber]++;
            }  
        }
    }
    COND_IntASD_ENABLE
}
//-------------------------------------------------------------------
void pathEncoderCycleProc(unsigned char relativeNumber)
{
unsigned char value;
    value = getDPShift(relativeNumber);
    if (value != 0)  
    {
//        simplePrintf("\npathEncoderCycleProc: num = %d, value = %d",relativeNumber,value);

        sendDPData(relativeNumber,value); 
    }
}
//-------------------------------------------------------------------
void trolleyPathCycleProc(void)
{
    pathEncoderCycleProc(TROLLEYSENSORNUMBER);
}
//-------------------------------------------------------------------
void scanerLCycleProc(void)
{
    pathEncoderCycleProc(SCANERLENGTHSENSORNUMBER);
}
//-------------------------------------------------------------------
void scanerHCycleProc(void)
{
    pathEncoderCycleProc(SCANERHEIGHTSENSORNUMBER);
}
//-------------------------------------------------------------------
void imitCycleProc(unsigned char relativeNumber) 
{
register unsigned char value;

    value = getImitShift(relativeNumber); 
    if (value != 0)
    {
        sendDPData(relativeNumber | IMITATORMASK,imitDPStep[relativeNumber]); 
    }
}
//-------------------------------------------------------------------
void imitTrolleyPathCycleProc(void)
{
    imitCycleProc(TROLLEYSENSORNUMBER);
}
//-------------------------------------------------------------------
void imitScanerLCycleProc(void)
{
    imitCycleProc(SCANERLENGTHSENSORNUMBER);
}
//-------------------------------------------------------------------
void imitScanerHCycleProc(void)
{
    imitCycleProc(SCANERHEIGHTSENSORNUMBER);
}
//-------------------------------------------------------------------
// д.б. пройден семафор DP_SEMAPHORE_xxx
// вызывать при переходе с датчика пути на ОБЫЧНЫЙ имитатор и наоборот
void redefineDPCycleProc(void)
{
unsigned char ii;
   for (ii=0; ii<QDP-1;ii++) 
   {
       if (ImitDPOn[ii] == IMITDPOFF)
       {
           if ( 
                ((mainShiftSensorNumber & IMITATORMASK) == 0) &&  ( ii ==  (mainShiftSensorNumber & SHIFTSENSORNUMBERMASK))  \
                 ||  (fIsTrolleyDP == FALSE)  && (ii == TROLLEYSENSORNUMBER)  \
                 ||  (fIsTrolleyDP == TRUE)  && (ii == SCANERLENGTHSENSORNUMBER)  \
              )  
           {  
               pShiftSensorProc[ii] = NULL;
           }
               else 
               {
                   switch(ii)
                   {
                       case TROLLEYSENSORNUMBER:
                           pShiftSensorProc[ii] = trolleyPathCycleProc;
                           break;
                       case SCANERLENGTHSENSORNUMBER:
                           pShiftSensorProc[ii] = scanerLCycleProc;
                           break;
                       case SCANERHEIGHTSENSORNUMBER:
                           pShiftSensorProc[ii] = scanerHCycleProc;
                           break;
                   } 
               }   
       } 
           else
           { // этот имитатор включен
               if ( (mainShiftSensorNumber & IMITATORMASK) &&  ( ii ==  (mainShiftSensorNumber & SHIFTSENSORNUMBERMASK)) ) 
               {
                   pShiftSensorProc[ii] = NULL;
               } 
                  else
                  {
                       switch(ii)
                       {
                           case TROLLEYSENSORNUMBER:
                               pShiftSensorProc[ii] = imitTrolleyPathCycleProc;
                               break;
                           case SCANERLENGTHSENSORNUMBER:
                               pShiftSensorProc[ii] = imitScanerLCycleProc;
                               break;
                           case SCANERHEIGHTSENSORNUMBER:
                               pShiftSensorProc[ii] = imitScanerHCycleProc;
                               break;
                       } 
                 }
           } 
   }
}
//-------------------------------------------------------------------
// при работе совместно с основным ДП
void scanerImitProc(void)
{
   if (scanerImitStage == SCANIMITINIT)
   {
            paramACounter = parameterA;
            scanerImitStage =  SCANIMITINC1;
            setImitDPStep(DPSTEPFORWARD,SCANERLENGTHSENSORNUMBER);
   }
       else 
       {       
           if (ImitDPOn[SCANERLENGTHSENSORNUMBER] == IMITDPWAIT)
           { 
               switch (scanerImitStage)
               {
                   case SCANIMITINC1:
                   {
                       paramACounter--;
                       if (paramACounter == 0)
                       {
                           paramACounter = parameterA << 1;
                          setImitDPStep(DPSTEPBACKWARD,SCANERLENGTHSENSORNUMBER);
                          scanerImitStage =  SCANIMITDEC;
                       } 
                       break;
                   }
                  case SCANIMITDEC:
                  {
                       paramACounter--;
                       if (paramACounter == 0)
                       {
                           paramACounter = parameterA;
                          setImitDPStep(DPSTEPFORWARD,SCANERLENGTHSENSORNUMBER);
                          scanerImitStage =  SCANIMITINC2;
                       }
                      break; 
                  }  
                  case SCANIMITINC2:
                  {
                       paramACounter--;
                       if (paramACounter == 0)
                       {
                           sendDPData(SCANERHEIGHTSENSORNUMBER | IMITATORMASK , imitDPStep[SCANERHEIGHTSENSORNUMBER]); 
                           parameterB--;
                          if (parameterB) 
                          { 
                              paramACounter = parameterA;
                              scanerImitStage =  SCANIMITINC1;
                          }
                              else doWhenScanerImitSwitchOFF();       //  выход из имитатора сканера
                       }
                      break; 
                  }
               }
            }  
        }
   imitScanerLCycleProc();

}
//-------------------------------------------------------------------
void sendDPData(unsigned char sensorNum, unsigned char sensorData)
{
tDPCOORDMESSAGEEX DPMessage;

    if ((fDPCoordDataExtended != 0) && (sensorNum == mainShiftSensorNumber) )
    {
        fillDPMessageHdr((void*)&DPMessage, TRUE);
        fillMessageByDPStepData(sensorNum, (void*)&DPMessage, sensorData, TRUE);
        WriteLE32U((unsigned char*)&DPMessage.Data.PathCoord, (DWORD)Pathcoord[sensorNum]);
        WriteLE32U((unsigned char*)&DPMessage.Data.DisplayCoord, displayCoord);
//
#ifdef OLD_PACKET_HEADER
        putmsg((unsigned char*)&DPMessage, szDPCoordMessageEx, NULL);
#else
        putmsg((unsigned char*)&DPMessage, szDPCoordMessageEx, attachMessageNumber);
#endif
//
    }
        else
        {
            register tDPCOORDMESSAGE *messagePtr = (tDPCOORDMESSAGE*)&DPMessage;
            fillDPMessageHdr((void*)&DPMessage, FALSE);
            fillMessageByDPStepData(sensorNum, (void*)&DPMessage, sensorData, FALSE);
            WriteLE32U((unsigned char*)&messagePtr->Data.PathCoord,(DWORD)Pathcoord[sensorNum]);
//
#ifdef OLD_PACKET_HEADER
            putmsg((unsigned char*)&DPMessage, szDPCoordMessage, NULL);
#else
            putmsg((unsigned char*)&DPMessage, szDPCoordMessage, attachMessageNumber);
#endif
//
        }
}
//-------------------------------------------------------------------
inline unsigned char getDPShift(UCHAR relativeNumber)
{
volatile register unsigned char val;
// признак того, что в outreg[] есть данные для чтения устанавливается по
// восходящему фронту /rd. Так что придется почитать еще раз, если считали ноль
     val = GET_DPx_VALUE(relativeNumber);
     if (val == 0) val = GET_DPx_VALUE(relativeNumber);
     return val;
}
//-------------------------------------------------------------------
inline unsigned char getImitShift(UCHAR relativeNumber)
{
unsigned char res;
    if  (ImitDPOn[relativeNumber] > IMITDPWAIT) 
    {                     
       res = imitDPStep[relativeNumber];
       ImitDPOn[relativeNumber] = IMITDPWAIT;
    }
        else res = 0;
    return res;
}
//-------------------------------------------------------------------
unsigned char getShiftSensorValue(UCHAR number)
{
    if (number & IMITATORMASK) 
    {
        return getImitShift(number & SHIFTSENSORNUMBERMASK);
    }
        else 
        {
            if ( (number == TROLLEYSENSORNUMBER ) && (fIsTrolleyDP == TRUE) || (number == SCANERLENGTHSENSORNUMBER ) && (fIsTrolleyDP == FALSE)  || (number == SCANERHEIGHTSENSORNUMBER ))
            {
                return getDPShift(number & SHIFTSENSORNUMBERMASK);
            }
               else 
               {
                   return 0; 
               }  
        }
}

//-------------------------------------------------------------------
void ustskBody(void)
{
    if (EnableBScan)
    {
#ifdef LARGE_BSCAN
        mScanAlarm();
#else
        read_bscan();
#endif
    }
    read_asd();

    DP_SEMAPHORE_ENTER
     for (register int ii=0; ii<QDP-1; ii++)
     {
         imitWriteDownCycleProc(ii);
         if (pShiftSensorProc[ii]  != NULL)  pShiftSensorProc[ii]();
     }
//
#ifdef DEVICE_EMULATION
     DP_SEMAPHORE_LEAVE
#endif
//
    if (EnableBScan)
    {
        volatile register int tDiff;
        volatile register int speedCalcPeriod = TimeToTick(cSpeedCalcTime);
        tDiff = get_tickdur(speedTimer);
        if (tDiff >= speedCalcPeriod)
        {
        tSPEEDMESSAGE speedMessage;

#ifndef FAKE_V_CALCULATION
        if ((mainShiftSensorNumber & IMITATORMASK) == 0)
        { // скорость считается по основному ДП, но не имитатору
            disintpld;
            lDiff = Pathcoord[mainShiftSensorNumber] - mainShiftSensorPathcoordMemorized;
            mainShiftSensorPathcoordMemorized = Pathcoord[mainShiftSensorNumber];
            COND_IntASD_ENABLE
            DP_SEMAPHORE_LEAVE
            if (lDiff < 0)
            {
                lDiff = (lDiff ^ 0xFFFFFFFF) + 1;
            }

// коррекция, если вдруг tDiff "сильнее" больше speedCalcPeriod, чтобы скорость не выпрыгивала
            lDiff = lDiff * speedCalcPeriod / tDiff;


//                   simplePrintf("dL = %d, dT = %d", lDiff, tDiff);

//
            if (lDiff > 0xFFFF) lDiff = 0xFFFF;
        }
            else
            {
                lDiff = 0;
                DP_SEMAPHORE_LEAVE
            }
#endif
            speedTimer = xTaskGetTickCount();
            fillMessageHeader((tLANMESSAGEHEADER*)&speedMessage, speedId, idBUM, speedSz);
            speedMessage.Speed = (unsigned short)lDiff;
#ifdef OLD_PACKET_HEADER
            putmsg((unsigned char*)&speedMessage, speedSz+hdrsize, NULL);
#else
            speedMessage.Header.NotUse = 0;
            putmsg((unsigned char*)&speedMessage, speedSz+hdrsize, attachMessageNumber);
#endif
        }
            else
            {
                DP_SEMAPHORE_LEAVE
            }
    }
        else
        {
            DP_SEMAPHORE_LEAVE
        }
/*
// отслеживание датчиков болтового стыка
    if (jointSensorDisable == FALSE)
    {
     register USHORT s;
         s = Rd_RegPLD(jointSensorAdr);
         if ((s ^ jointSensorState) & jointSensorMask)
         {
            if ((s ^ jointSensorState) & jointSensorMaskL)
            {
                if (s & jointSensorMaskL) make_jointSensor(SideLeft,TRUE);
                    else make_jointSensor(SideLeft,FALSE);
            }
           if ((s ^ jointSensorState) & jointSensorMaskR)
            {
                if (s & jointSensorMaskR) make_jointSensor(SideRight,TRUE);
                    else make_jointSensor(SideRight,FALSE);
            }
            jointSensorState = s;
         }
    }
*/
//
#ifndef AC_dis
   TakeSemaphore(s_ACTH);
   disintpld;
   switch(ACCalcStage)
   {
/*
       case ACTHCALC_CNTQUERY:
           simplePrintf("\n ACTHCALC_CNTQUERY");
           break;

       case ACTHCALC_WAITFORDATA:
           simplePrintf("\n ACTHCALC_WAITFORDATA");
           break;
*/
       case ACTHCALC_COMPUTEQUERY:
       {
//                  simplePrintf("\n ACTHCALC_COMPUTEQUERY");

           if ( (gatheringSumCnt > (cGATHERINGSUMDURATION - ACTHRARRAY_DEPTH)) || (gatheringSumCnt ==0) )
           {
               shiftACDataSums();
//                      printACDataSums(1, 1, 2);
           }
               else
               {
                   if (gatheringSumCnt == (cGATHERINGSUMDURATION - ACTHRARRAY_DEPTH))
                   {
                       sortACDataSumS();
                   }
                   addACDataSums();
//                          printACDataSums(1, 1, 2);
               }

           if (gatheringSumCnt != 0) gatheringSumCnt--;
           if (gatheringSumCnt != 0)
           {
               acThCalcPeriodCnt = cACSumGatheringPeriodByCycle;
               ACCalcStage = ACCALC_NEED_TO_START;
               break;
           }
               else
               {
                   ACCalcStage = ACTHCALC_COMPUTEEXEC;
                   break; // именно так!
               }
       }
       case ACTHCALC_COMPUTEEXEC:
       {
//                simplePrintf("\n ACTHCALC_COMPUTEEXEC");
           acThCalcPeriodCnt = ACThCalcPeriodByCycle;
           Wr_RegPLD(ACControlReg, ACControlValue_EVAL);
           ACCalcStage = ACCALC_NEED_TO_START;
           calculateAndSetTh(acThPercent);
           break;
       }
       case ACCALC_NEED_TO_START:
       {
           Wr_RegPLD(ACSumStartReg, ACSumStartValue);
           ACCalcStage = ACCALC_WAITFORDATA;
           break;
       }
   }
   COND_IntASD_ENABLE
   xSemaphoreGive(s_ACTH);
#endif
}
//
#ifndef DEVICE_EMULATION
void ustsk(void *ppar)
{
//
BOOL tcpEst;
  
   smprintf("\nustsk started");

    tcpEst = FALSE;
    if ((versionInfoTimeoutPar ==  PARAM_UNDEFINED) || (versionInfoTimeoutPar <  cVersionInfoTimeoutMin) )  versionInfoTimeout =  PARAM_UNDEFINED;
        else versionInfoTimeout = versionInfoTimeoutPar;
    if ((jointSensorDisablePar == PARAM_UNDEFINED) || (jointSensorDisablePar == 0)) jointSensorDisable = FALSE;
        else jointSensorDisable = TRUE;

#ifdef us46emu
     tempbuf[0] = tempbuf[(tempbuf_size>>1)] = idArazv;
     tempbuf[1] = tempbuf[(tempbuf_size>>1)+1] = idBUM;
//
     tempbuf[2] = tempbuf[(tempbuf_size>>1)+2] = szArazv & 0xFF;
     tempbuf[3] = tempbuf[(tempbuf_size>>1)+3] = szArazv >> 8;
#endif

#ifndef AC_dis
  pAcThLine0 =  (tACThLine*) memalloc(sizeof(tACThLine) * 2);
  if (pAcThLine0 == NULL)
  {
      simplePrintf("\n ustsk: невозможно выделить память под pAcThLine0. Остановлено"); 
      while(1) vTaskDelay(tormoz);
  }
      else
      {
          pAcThLine1 = (tACThLine*) ((unsigned char*)pAcThLine0 + sizeof(tACThLine));
      }
  AScanSumThTablesInitialize();

//
  pACMaxSums = (ACTHRARRAY*)memalloc(sizeof(ACTHRARRAY));
  if  (pACMaxSums == NULL)
  {
      simplePrintf("\n ustsk: невозможно выделить память под pACMaxSums. Остановлено");
      while(1) vTaskDelay(tormoz);
  }
//
  {
      unsigned int ii;
      unsigned char *pB;
      pB = (unsigned char *)pACMaxSums;
      for (ii=0; ii<sizeof(ACTHRARRAY); ii++) pB[ii] = 0;
  }
#endif
//
  while(1)
  {
      if  (ts.getcpstate(sh) == UIP_ESTABLISHED) 
      {
          if (tcpEst == FALSE) 
          {
               tcpEst = TRUE;
               versionInfoSendTick = xTaskGetTickCount();
          }
          vTaskDelay(1);
          read_ascan(0);  // liniya 1
          read_ascan(1);  // liniya 2
#ifndef us46emu 
          ustskBody();
#endif
//
           if ((versionInfoTimeout != PARAM_UNDEFINED) &&  (get_tickdur(versionInfoSendTick) > (TimeToTick(versionInfoTimeout)) ) )
           {
              ts.abortconn(sh,FALSE,0);
              ASD_Off();
              StopAScan();
              StopBScan();
              tcpEst = FALSE;
           }
      }
          else 
          {  
             vTaskDelay(50);
              tcpEst = FALSE;
          }

/*
    if (dx != dxMem)
    {
       simplePrintf("\n dx = %d",dx);
       dxMem = dx;
    }

    if (dy != dyMem)
    {
       simplePrintf("\n dy = %d",dy);
       dyMem = dy;
    }
    if (dz != dzMem)
    {
       simplePrintf("\n dz = %d",dz);
       dzMem = dz;
    }
*/

  } // while
}
#else
void ustsk(void *ppar)
{
    read_ascan(0);  // liniya 1
    read_ascan(1);  // liniya 2
    ustskBody();
}
#endif
//-------------------------------------------------------------------





