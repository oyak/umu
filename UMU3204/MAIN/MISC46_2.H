#ifndef _misc46_2h
#define _misc46_2h
// исходный misc46_1.h
// добавлены идентификаторы адресов регистров инициализации PLDVerId
// misc46_1.h - ?
// misc46.c
// добавлены константы extramstart DPValueAdr из misc464.h 


#ifndef DEVICE_EMULATION
#define extramstart (0x5000 << 1)
#endif

// регистры датчика пути, старший байт ПЛИС
#define LENGTHDPVALUEADR  (ramstart + (0x13D0 << 1) )    // чтение,адрес регистра "значение основного датчика пути"  - после чтения значение регистра сбрасывается
#define HEIGTDPVALUEADR  (ramstart + (0x13D1 << 1) )    // чтение,адрес регистра "значение дополнительного датчика пути"  - после чтения значение регистра сбрасывается
#define DPCONTROLREGADR  (ramstart + (0x13DF << 1) )    // запись,адрес регистра "DPControlReg"  
// для основного датчика пути:  значения 0/1
// бит 0 - переключение: датчик разъема ДП/датчик сканера 
#define  SETSCANERDPMAJORBIT (1<<8)
// бит 1   отключить/включить делитель на 3
#define  SETDIVIDER3BIT (1<<9)
//
#define STOP_DP_MACHINE_BIT (1<<10) //




// при чтении
// 
#define PLDVerIDMinAdr  (ramstart + (0x1400 << 1) )    // адрес регистра [7..0] "младший байт идентификатора версии ПЛИС"
#define PLDVerIDMajAdr  (ramstart + (0x1401 << 1) )    // адрес регистра [7..0] "старший байт идентификатора версии ПЛИС"

#define jointSensorAdr  (ramstart + (0x1391 << 1) )       //  состояние датчиков болтового стыка и стрелочного перевода
#define jointSensorMask  (0x101)
#define jointSensorMaskR  (0x100)
#define jointSensorMaskL  (0x1)



#ifndef DEVICE_EMULATION
inline void Wr_RegPLD(DWORD regAddr, USHORT val)
{
         *((USHORT*) regAddr) = val;
}
//----------------------------------------------------------------------------------------------------
//
// чтение регистра ПЛИС
inline USHORT Rd_RegPLD(DWORD regAddr)
 {
         return  *((USHORT*) regAddr);
}
#endif
//----------------------------------------------------------------------------------------------------
// gets ammount of ticks passed till timestartick
int get_tickdur(int timestartick);
//
// caluculates  ammount of ticks passed till timestartick untill timeendtick
int calc_tickdur(int timestartick, int timeendtick);

#endif
