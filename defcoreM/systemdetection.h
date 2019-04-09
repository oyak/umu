#ifndef SYSTEMDETECTION_H
#define SYSTEMDETECTION_H

/*
   The operating system, must be one of: (DEFCORE_OS_x)

     DARWIN   - Any Darwin system
     MAC      - OS X and iOS
     OSX      - OS X
     IOS      - iOS
     MSDOS    - MS-DOS and Windows
     OS2      - OS/2
     OS2EMX   - XFree86 on OS/2 (not PM)
     WIN32    - Win32 (Windows 2000/XP/Vista/7 and Windows Server 2003/2008)
     WINCE    - WinCE (Windows CE 5.0)
     WINRT    - WinRT (Windows 8 Runtime)
     CYGWIN   - Cygwin
     SOLARIS  - Sun Solaris
     HPUX     - HP-UX
     ULTRIX   - DEC Ultrix
     LINUX    - Linux [has variants]
     FREEBSD  - FreeBSD [has variants]
     NETBSD   - NetBSD
     OPENBSD  - OpenBSD
     BSDI     - BSD/OS
     IRIX     - SGI Irix
     OSF      - HP Tru64 UNIX
     SCO      - SCO OpenServer 5
     UNIXWARE - UnixWare 7, Open UNIX 8
     AIX      - AIX
     HURD     - GNU Hurd
     DGUX     - DG/UX
     RELIANT  - Reliant UNIX
     DYNIX    - DYNIX/ptx
     QNX      - QNX [has variants]
     QNX6     - QNX RTP 6.1
     LYNX     - LynxOS
     BSD4     - Any BSD 4.4 system
     UNIX     - Any UNIX BSD/SYSV system
     ANDROID  - Android platform

   The following operating systems have variants:
     LINUX    - both DEFCORE_OS_LINUX and DEFCORE_OS_ANDROID are defined when building for Android
              - only DEFCORE_OS_LINUX is defined if building for other Linux systems
     QNX      - both DEFCORE_OS_QNX and DEFCORE_OS_BLACKBERRY are defined when building for Blackberry 10
              - only DEFCORE_OS_QNX is defined if building for other QNX targets
     FREEBSD  - DEFCORE_OS_FREEBSD is defined only when building for FreeBSD with a BSD userland
              - DEFCORE_OS_FREEBSD_KERNEL is always defined on FreeBSD, even if the userland is from GNU
*/

#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#define DEFCORE_OS_DARWIN
#define DEFCORE_OS_BSD4
#ifdef __LP64__
#define DEFCORE_OS_DARWIN64
#else
#define DEFCORE_OS_DARWIN32
#endif
#elif defined(ANDROID)
#define DEFCORE_OS_ANDROID
#define DEFCORE_OS_LINUX
#elif defined(__CYGWIN__)
#define DEFCORE_OS_CYGWIN
#elif !defined(SAG_COM) && (!defined(WINAPI_FAMILY) || WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#define DEFCORE_OS_WIN32
#define DEFCORE_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#if defined(WINCE) || defined(_WIN32_WCE)
#define DEFCORE_OS_WINCE
#elif defined(WINAPI_FAMILY)
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#define DEFCORE_OS_WINPHONE
#define DEFCORE_OS_WINRT
#elif WINAPI_FAMILY == WINAPI_FAMILY_APP
#define DEFCORE_OS_WINRT
#else
#define DEFCORE_OS_WIN32
#endif
#else
#define DEFCORE_OS_WIN32
#endif
#elif defined(__sun) || defined(sun)
#define DEFCORE_OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
#define DEFCORE_OS_HPUX
#elif defined(__ultrix) || defined(ultrix)
#define DEFCORE_OS_ULTRIX
#elif defined(sinix)
#define DEFCORE_OS_RELIANT
#elif defined(__native_client__)
#define DEFCORE_OS_NACL
#elif defined(__linux__) || defined(__linux)
#define DEFCORE_OS_LINUX
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(__FreeBSD_kernel__)
#ifndef __FreeBSD_kernel__
#define DEFCORE_OS_FREEBSD
#endif
#define DEFCORE_OS_FREEBSD_KERNEL
#define DEFCORE_OS_BSD4
#elif defined(__NetBSD__)
#define DEFCORE_OS_NETBSD
#define DEFCORE_OS_BSD4
#elif defined(__OpenBSD__)
#define DEFCORE_OS_OPENBSD
#define DEFCORE_OS_BSD4
#elif defined(__bsdi__)
#define DEFCORE_OS_BSDI
#define DEFCORE_OS_BSD4
#elif defined(__sgi)
#define DEFCORE_OS_IRIX
#elif defined(__osf__)
#define DEFCORE_OS_OSF
#elif defined(_AIX)
#define DEFCORE_OS_AIX
#elif defined(__Lynx__)
#define DEFCORE_OS_LYNX
#elif defined(__GNU__)
#define DEFCORE_OS_HURD
#elif defined(__DGUX__)
#define DEFCORE_OS_DGUX
#elif defined(__QNXNTO__)
#define DEFCORE_OS_QNX
#elif defined(_SEQUENT_)
#define DEFCORE_OS_DYNIX
#elif defined(_SCO_DS) /* SCO OpenServer 5 + GCC */
#define DEFCORE_OS_SCO
#elif defined(__USLC__) /* all SCO platforms + UDK or OUDK */
#define DEFCORE_OS_UNIXWARE
#elif defined(__svr4__) && defined(i386) /* Open UNIX 8 + GCC */
#define DEFCORE_OS_UNIXWARE
#elif defined(__INTEGRITY)
#define DEFCORE_OS_INTEGRITY
#elif defined(VXWORKS) /* there is no "real" VxWorks define - this has to be set in the mkspec! */
#define DEFCORE_OS_VXWORKS
#elif defined(__MAKEDEPEND__)
#else
#error "Defcore has not been ported to this OS"
#endif

#if defined(DEFCORE_OS_WIN32) || defined(DEFCORE_OS_WIN64) || defined(DEFCORE_OS_WINCE) || defined(DEFCORE_OS_WINRT)
#define DEFCORE_OS_WIN
#endif

#if defined(DEFCORE_OS_WIN)
#undef DEFCORE_OS_UNIX
#elif !defined(DEFCORE_OS_UNIX)
#define DEFCORE_OS_UNIX
#endif

#endif  // SYSTEMDETECTION_H
