// исходный misc132.c
// добавлены ReadBE16U, WriteBE16U

//  misc132.c
// добавлены ReadLE16U, WriteLE16U

#include "mvnewa.h"
//

DWORD  ReadLE32U (volatile  UCHAR *pmem)
{
DWORD  val;

    ((UCHAR *)&val)[0] = pmem[0];
    ((UCHAR *)&val)[1] = pmem[1];
    ((UCHAR *)&val)[2] = pmem[2];
    ((UCHAR *)&val)[3] = pmem[3];

    return (val);
}
//-------------------------------------------------------------------
void  WriteLE32U (volatile  UCHAR  *pmem,
                            DWORD   val)
{
    pmem[0] = ((UCHAR *)&val)[0];
    pmem[1] = ((UCHAR *)&val)[1];
    pmem[2] = ((UCHAR *)&val)[2];
    pmem[3] = ((UCHAR *)&val)[3];
}

DWORD  ReadBE32U (volatile  UCHAR *pmem)
{
DWORD  val;

    ((UCHAR *)&val)[0] = pmem[3];
    ((UCHAR *)&val)[1] = pmem[2];
    ((UCHAR *)&val)[2] = pmem[1];
    ((UCHAR *)&val)[3] = pmem[0];

    return (val);
}
//-------------------------------------------------------------------
void  WriteBE32U (volatile  UCHAR  *pmem,
                            DWORD   val)
{
    pmem[0] = ((UCHAR *)&val)[3];
    pmem[1] = ((UCHAR *)&val)[2];
    pmem[2] = ((UCHAR *)&val)[1];
    pmem[3] = ((UCHAR *)&val)[0];
}
//-------------------------------------------------------------------
USHORT ReadLE16U (volatile  UCHAR *pmem)
{
USHORT val;
    ((UCHAR *)&val)[0] = pmem[0];
    ((UCHAR *)&val)[1] = pmem[1];
    return (val);
}
//-------------------------------------------------------------------
void  WriteLE16U (volatile  UCHAR  *pmem,
                            USHORT   val)
{
    pmem[0] = ((UCHAR *)&val)[0];
    pmem[1] = ((UCHAR *)&val)[1];
}
//-------------------------------------------------------------------
USHORT  ReadBE16U (volatile  UCHAR *pmem)
{
USHORT  val;

    ((UCHAR *)&val)[0] = pmem[1];
    ((UCHAR *)&val)[1] = pmem[0];
    return (val);
}
//-------------------------------------------------------------------
void  WriteBE16U (volatile  UCHAR  *pmem,
                            USHORT   val)
{
    pmem[2] = ((UCHAR *)&val)[1];
    pmem[3] = ((UCHAR *)&val)[0];
}
