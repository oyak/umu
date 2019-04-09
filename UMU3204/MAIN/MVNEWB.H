// ¨§ δ ©«  mvnewa.h
// ξοπεδελενϋ ςθο HANDLE θ INVALID_HANDLE_VALUE
#ifndef _MVNEW1_H
#define _MVNEW1_H

#define FALSE      0
#define TRUE       1

typedef int                 BOOL;
typedef unsigned char       UCHAR;
typedef unsigned long       ULONG;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned short      USHORT;
typedef unsigned int        UINT;
typedef unsigned long       DWORD;
//
#ifndef DEVICE_EMULATION
typedef int                 HANDLE;
#endif

#ifndef LOBYTE
#define LOBYTE(w)   ((BYTE)(w))
#endif

#ifndef HIBYTE
#define HIBYTE(w)   ((BYTE)((WORD)(w) >> 8))
#endif

#ifndef LOWORD
#define LOWORD(l)   ((WORD)(l))
#endif

#ifndef HIWORD
#define HIWORD(l)   ((WORD)((DWORD)(l) >> 16))
#endif

//
#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE  (-1)
#endif
//
#endif
