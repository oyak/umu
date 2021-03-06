#ifndef _misc46_2h
#define _misc46_2h
// �������� misc46_1.h
// ��������� �������������� ������� ��������� ������������� PLDVerId
// misc46_1.h - ?
// misc46.c
// ��������� ��������� extramstart DPValueAdr �� misc464.h 

#define extramstart (0x5000 << 1)

// �������� ������� ����, ������� ���� ����
#define LENGTHDPVALUEADR  (ramstart + (0x13D0 << 1) )    // ������,����� �������� "�������� ��������� ������� ����"  - ����� ������ �������� �������� ������������
#define HEIGTDPVALUEADR  (ramstart + (0x13D1 << 1) )    // ������,����� �������� "�������� ��������������� ������� ����"  - ����� ������ �������� �������� ������������
#define DPCONTROLREGADR  (ramstart + (0x13DF << 1) )    // ������,����� �������� "DPControlReg"  
// ��� ��������� ������� ����:  �������� 0/1
// ��� 0 - ������������: ������ ������� ��/������ ������� 
#define  SETSCANERDPMAJORBIT (1<<8)
// ��� 1   ���������/�������� �������� �� 3
#define  SETDIVIDER3BIT (1<<9)
//
#define STOP_DP_MACHINE_BIT (1<<10) //




// ��� ������
// 
#define PLDVerIDMinAdr  (ramstart + (0x1400 << 1) )    // ����� �������� [7..0] "������� ���� �������������� ������ ����"
#define PLDVerIDMajAdr  (ramstart + (0x1401 << 1) )    // ����� �������� [7..0] "������� ���� �������������� ������ ����"

#define jointSensorAdr  (ramstart + (0x1391 << 1) )       //  ��������� �������� ��������� ����� � ����������� ��������
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
// ������ �������� ����
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
