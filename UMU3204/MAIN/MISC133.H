// added ReadLE16U WriteLE16U defintition
//
#ifndef _misc133h
#define _misc133h
//
#include "mvnewa.h"
//
DWORD  ReadLE32U (volatile  UCHAR *pmem);

//-------------------------------------------------------------------
void  WriteLE32U (volatile  UCHAR  *pmem,DWORD   val);

//-------------------------------------------------------------------
DWORD  ReadBE32U (volatile  UCHAR *pmem);

//-------------------------------------------------------------------
void  WriteBE32U (volatile  UCHAR  *pmem, DWORD   val);
//-------------------------------------------------------------------
USHORT  ReadLE16U (volatile  UCHAR *pmem);
//-------------------------------------------------------------------
void  WriteLE16U (volatile  UCHAR  *pmem,
                            USHORT   val);
//-------------------------------------------------------------------
USHORT  ReadBE16U (volatile  UCHAR *pmem);
//-------------------------------------------------------------------
void  WriteBE16U (volatile  UCHAR  *pmem,
                            USHORT   val);

#endif



