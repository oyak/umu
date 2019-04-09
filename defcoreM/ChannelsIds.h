#ifndef CHANNELIDS_H
#define CHANNELIDS_H

#define N0EMS 0x01
#define N0EMS2 0x02
#define F58EB 0x03
#define F58EW 0x04
#define F58EU 0x05
#define F58ELW 0x06
#define F58ELU 0x07
#define B58EB 0x08
#define B58EW 0x09
#define B58EU 0x0A
#define B58ELW 0x0B
#define B58ELU 0x0C
#define F58MB 0x0D
#define F58MW 0x0E
#define F58MU 0x0F
#define F58MLW 0x10
#define F58MLU 0x11
#define B58MB 0x12
#define B58MW 0x13
#define B58MU 0x14
#define B58MLW 0x15
#define B58MLU 0x16
#define F70E 0x17
#define B70E 0x18
#define F42E 0x19
#define B42E 0x1A
#define F42EE 0xBD
#define B42EE 0xBE
#define F45E 0x1B
#define B45E 0x1C
#define F22E 0xA1
#define B22E 0xA2
#define H0E 0x1D
#define H0MS 0x1E
#define H45 0x1F
#define H50 0x20
#define H58 0x21
#define H65 0x22
#define H70 0x23
#define LH0E 0x24
#define LH0MS 0x25
#define LH45 0x26
#define LH50 0x27
#define LH58 0x28
#define LH65 0x29
#define LH70 0x2A
#define RH0E 0x2B
#define RH0MS 0x2C
#define RH45 0x2D
#define RH50 0x2E
#define RH58 0x2F
#define RH65 0x30
#define RH70 0x31
#define F70LPE 0x32
#define F70RPE 0x33
#define B70LPM 0x34
#define S70LPE 0x35
#define S70RPE 0x36
#define B70LPE 0x37
#define B70RPE 0x38
#define N55MSLW 0x39   // Канал большого колесного преобразователя ?
#define N55MSLU 0x3A   // Канал большого колесного преобразователя ?
#define F45E_WP 0x3B   // Канал большого колесного преобразователя
#define B45E_WP 0x3C   // Канал большого колесного преобразователя
#define N0EMS_WP 0x3D  // Канал большого колесного преобразователя
#define H0E_5MGz 0x3E
#define H0MS_5MGz 0xB8
#define TEST_1 0xB9
#define TEST_2 0xBA
#define H0E_UIC60 0xBB   // Канал ручного контроля 0° ЭХО для UIC 60
#define H0MS_UIC60 0xBC  // Канал ручного контроля 0° ЗТМ для UIC 60

#define F65E_WP 0xA9  // Каналы большого колесного преобразователя
#define F65EW_WP 0xAA
#define F65EU_WP 0xAB
#define B65E_WP 0xAC
#define B65EW_WP 0xAD
#define B65EU_WP 0xAE

#define CHANNELS_TOTAL \
    0x3D  // Для алгоритма фильтрации сигналов!
          // Используется для задания размера массива
          // Модифицировать при увеличении количества каналов

#endif // CHANNELIDS_H
