#ifndef pconsth46
#define pconsth46

#include "FreeRTOSConfig.h"

#define BUM3204
#define TWO_COLOR_LED
// создать для МОИ версии 3 (БУМ с двуцветным светодиодом на боковой панели) - светодиоды включаются лог.1

#define XNumber 46
#define asmblnum 3

#ifndef _DEBUG

#ifdef TWO_COLOR_LED
#define YNumber  9
#else
#define YNumber  7
#endif

#else
#define YNumber  134
#endif

#define ZNumber  7


//define CONFIGURATION_PRINT      // гјўпҐЁ о  лЇ­п¬ј пІ®в¦­оЇ±иЎЄп®ґй¤іб·ЁиЎЏОЌ
//define DISABLE_FTP_SERVER
//define IGNORE_CODE_SUM           // оҐ аЇ¤йі»гЎІ лЇ­п¬јоґѕ н­і ж«іж© а±®д± н­»
//define SUPRESS_REPROGRAMMING
//define START_CONSOLE_ONLY     // а® г««ж®Ё а¦°ж¶®е©І бЁі о  лЇ­п¬ј


//--------------------------------------------------------------------------------------------------------	
//
#define DNumber  asmblnum
//
#define version_number (( (XNumber & 0xFF) << 24) | ( (YNumber & 0xFF) << 16) | ( (ZNumber & 0xFF) << 8) | (DNumber & 0xFF))
//
#ifdef BUM3204
#define RMII_BUM     // е¬ї В“М  RMII й®Іж±ґжЄ±п¬Ќ
#endif

#define iramstart 0x40000000
//
//
#define uartxsend uart1send
#define uartxinit uart1init
#define uartxtrstate UART1sendstate
#define uartxrdlock UART1_intRX_dis
#define uartxrdunlock UART1_intRX_en

//
//define uartxsend uart0send
//define uartxinit uart0init
//define uartxtrstate UART0sendstate

#ifdef BUM3204
#define conuartinit uart0init
#define conUartDeInit uart0DeInit

#define conuartxrdlock UART0_intRX_dis
#define conuartxrdunlock UART0_intRX_en
#define conuart 0
#else
#define conuartinit uart1init
#define conUartDeInit uart1DeInit
#define conuart 1
#endif
//
#ifndef DEVICE_EMULATION
#define iramstart 0x40000000
#define ramstart  0x80000000
#define ramvolume 0x7FFFF
#endif
#define rambegin (UCHAR*)ramstart
//
//
#define tres int

//define OStick_delay_multiplier 1
#define tormoz 1  // delay in ticks for user software if system service can not execute
// require action and have to wait
//
#define TASK_MAX_PRIORITY (tskIDLE_PRIORITY+5)
#define TASK_COMMON_PRIORITY (tskIDLE_PRIORITY+3)
#define TASK_MIN_PRIORITY (tskIDLE_PRIORITY+1)
//
// constants for mmanager module

#define userMemStartAddr 0x40008500  //
#define userMemSize   (30 * 1024)
#define userPageSize  64 

#define systemMemStartAddr 0x40004900  //
#define systemMemSize   (userMemStartAddr - systemMemStartAddr)
#define systemPageSize  8 


//
// enet speed
#define FIX_SPEED          SPEED_100 // SPEED_10
//
//
//
#ifdef  BUM3204         // defines using BUM3204 instead of kit

#ifndef RMII_BUM
#define HOSTCLOCK_25
#define MIIM
#endif
//
#else  // for kit
//    define HOSTCLOCK_50

#endif
//
#define tFTPwaits 10000       // г±Ґнї п§ЁеЎ­йї б®®г«Ё дЂ§иЎ± FTP-л¬Ёж®Іп¬ вЎ¬й¬«йІҐлґ­еЎµ


#ifdef BUM3204
// BUM defintition
//define us46emu //emulaiton instead of PLDs; вЎ±мґ·бҐ В“М rbf-бЄ« оҐ иЎЈзЎҐ, LED1 гјЄмї·б¦І

//define msg0x82_dis    // disable 0x82 message generation - MAX amp and delay AScan
#define LARGE_BSCAN
//define envelop_dis    // disable 0x85 message generation - envelops
#define oldmscanformat // use mscan 0x71 format
//
//define ADC_CONST                        // аЇ«б¤  и® ж®Ёе¬ йі о®®еЎ± VП аЇ±-ој¬ е¬ї лЎ¦еЇ© п±®о»Ќ
#define ADC_CONST_VALUE 0x7F7F   // и® ж®ЁеЎЅп© аЇ±-оЇ©
//
#define ONE_CHANNEL_ASCAN_ONLY  // приходит только одна А-развертка (сторона-линия-такт)
//
//define DISABLE_ASD_ISR_MINIMAL_PERIOD_CONSTRICTION // немедленная реакция на прерывание ПЛИС, т.е. нет использования
// таймера, чтобы ограничить минимальный период обработки прерываний при малой длине цикла прозвучивания
// при прерывании от ПЛИС вызывается обработчик IntASD()
//
//define LEVEL_SENSITIVE_PLD_INT_SERVICE


// define OLD_PACKET_HEADER

//define AC_dis // не настраиваем АК не передаем данные об АК

//define LENGTH_PATH_ENCODER_DIVIDER_OFF // по-умолчанию задать значение параметра lengthPathEncoderDividerOffPar равным 1


#define paramsfilename "PARAMS.INI"    
#define RBFFILENAME "bum3204.rbf"


#define PROGRAMMATORFILE "prog.bin"    
#define PROGRAMFILE "bum3204.bin"    
#define PROGRAMMATOR_START_ADDRESS_PTR 0x8000a04C
//#define PROGRAMMATOR_START_ADDRESS_PTR 0x4000004C

#endif //  ifdef BUM3204
//
#define console_sh 1
#define PRINT_STARTON  // vkl. dbgout when program starting otherwise dbgout is off

/*
#ifdef _DEBUGPRINT
#define PRINT_STARTON  // vkl. dbgout when program starting otherwise dbgout is off
#endif
*/
//
#define Set_CSPLD
#define Reset_CSPLD
//------------------------------------------------------------------------------------------------------------------------
//define _armftp_cpp_prn
//define _arp_c_prn
//define _dhisr_c_prn
//define _ldparams46_cpp_prn
//define _tcp_cpp_prn
//define _tcpudp_c_prn
//define _umuc_cpp_prn
//define _us46_cpp_prn

//define _test_message_numeration_integrity

#ifdef _DEBUG 
#if Y < 128
#error Y must be equal or greater than 128 when _DEBUG
#endif  
#else
#if Y >= 128
#error Y must be less than 128 when _DEBUG is not defined
#endif
#endif
//

#ifndef _DEBUG

#if (configUSE_TRACE_FACILITY == 1)
#error configUSE_TRACE_FACILITY in the FreeRTOSConfig.h file the may be defined when _DEBUG has been defined too
#endif
//
#ifdef SUPRESS_REPROGRAMMING 
#error SUPRESS_REPROGRAMMING may be defined when _DEBUG
#endif

#ifdef DISABLE_FTP_SERVER
#error DISABLE_FTP_SERVER may be defined when _DEBUG
#endif

#ifdef IGNORE_CODE_SUM 
#error IGNORE_CODE_SUM  may be defined when _DEBUG
#endif

#ifdef SUPRESS_REPROGRAMMING 
#error SUPRESS_REPROGRAMMING may be defined when _DEBUG
#endif

#ifdef START_CONSOLE_ONLY
#error START_CONSOLE_ONLY may be defined when _DEBUG
#endif
#endif

//
#endif



