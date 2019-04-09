#ifndef _mtypesh
#define _mtypesh

typedef void (*vfuncv)(void);
typedef void (*vfunc18)(UCHAR);
typedef void (*vfunc1ps8)(char*);
typedef void (*vfunc28)(UCHAR,UCHAR);
typedef void (*vfunc38)(UCHAR,UCHAR,UCHAR);
//
typedef void (*vfunc2pv)(void*,void*);
typedef void (*vfuncpv)(void*);

typedef UCHAR (*u8funcpvu8)(void*,UCHAR);
typedef short (*s16funcpvu8)(void*,UCHAR);
typedef short (*s16funcpv)(void*);

typedef int (*sifuncv)(void);  
typedef int (*sifuncpv)(void*);  


#ifndef DEVICE_EMULATION
typedef unsigned int uint32_t;
typedef int int32_t;
#endif


#endif
