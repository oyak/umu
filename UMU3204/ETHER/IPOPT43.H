#ifndef _IPOPT43H
#define _IPOPT43H

#ifndef UIP_LITTLE_ENDIAN
#define UIP_LITTLE_ENDIAN  3412
#endif /* UIP_LITTLE_ENDIAN */
#ifndef UIP_BIG_ENDIAN
#define UIP_BIG_ENDIAN     1234
#endif /* UIP_BIG_ENDIAN */

#include "uipconf.h"


// These configuration options can be used for setting the IP address
// settings statically, but only if UIP_FIXEDADDR is set to 1. The
// configuration options for a specific node includes IP address,
// netmask and default router as well as the Ethernet address. The
// netmask, default router and Ethernet address are appliciable only
// if uIP should be run over Ethernet.
//-------------------------------------------------------------------
// Determines if uIP should use a fixed IP address or not.
// If uIP should use a fixed IP address, the settings are set in the
// uipopt.h file. If not, the macros uip_sethostaddr(),
// uip_setdraddr() and uip_setnetmask() should be used instead.
//
#define UIP_FIXEDADDR    0
//-------------------------------------------------------------------
#ifdef UIP_CONF_PINGADDRCONF
//
// Ping IP address asignment.
// uIP uses a "ping" packets for setting its own IP address if this
// option is set. If so, uIP will start with an empty IP address and
// the destination IP address of the first incoming "ping" (ICMP echo)
// packet will be used for setting the hosts IP address.
// note This works only if UIP_FIXEDADDR is 0.
//
//
#define UIP_PINGADDRCONF UIP_CONF_PINGADDRCONF

#else /* UIP_CONF_PINGADDRCONF */
#define UIP_PINGADDRCONF 0
#endif /* UIP_CONF_PINGADDRCONF */

//-------------------------------------------------------------------
//
// Specifies if the uIP ARP module should be compiled with a fixed
// Ethernet MAC address or not.
// If this configuration option is 0, the macro uip_setethaddr() can
// be used to specify the Ethernet address at run-time.
//
#define UIP_FIXEDETHADDR 1

#define UIP_ETHADDR0    0x00  /**< The first octet of the Ethernet
                                 address if UIP_FIXEDETHADDR is
                                 1. \hideinitializer */
#define UIP_ETHADDR1    0xFF  /**< The second octet of the Ethernet
                                 address if UIP_FIXEDETHADDR is
                                 1. \hideinitializer */
#define UIP_ETHADDR2    0xFF  /**< The third octet of the Ethernet
                                 address if UIP_FIXEDETHADDR is
                                 1. \hideinitializer */
#define UIP_ETHADDR3    0xFF  /**< The fourth octet of the Ethernet
                                 address if UIP_FIXEDETHADDR is
                                 1. \hideinitializer */
#define UIP_ETHADDR4    0xFF  /**< The fifth octet of the Ethernet
                                 address if UIP_FIXEDETHADDR is
                                 1. \hideinitializer */
#define UIP_ETHADDR5    0xFF  /**< The sixth octet of the Ethernet
                                 address if UIP_FIXEDETHADDR is
                                 1. \hideinitializer */

//-------------------------------------------------------------------
// The IP TTL (time to live) of IP packets sent by IP.
//This should normally not be changed.
#define UIP_TTL         1
//-------------------------------------------------------------------
// Turn on support for IP packet reassembly.
// uIP supports reassembly of fragmented IP packets. This features
// requires an additonal amount of RAM to hold the reassembly buffer
// and the reassembly code size is approximately 700 bytes.  The
// reassembly buffer is of the same size as the uip_buf buffer
// (configured by UIP_BUFSIZE).
// note IP packet reassembly is not heavily tested.
#define UIP_REASSEMBLY 0
//-------------------------------------------------------------------
// The maximum time an IP fragment should wait in the reassembly
// buffer before it is dropped.
#define UIP_REASS_MAXAGE 40
//-------------------------------------------------------------------
#ifdef UIP_CONF_UDP
// Toggles wether UDP support should be compiled in or not.
#define UIP_UDP UIP_CONF_UDP
#else /* UIP_CONF_UDP */
#define UIP_UDP           0
#endif /* UIP_CONF_UDP */
//------------------------------------------------------------------------
#ifdef UIP_CONF_UDP_CHECKSUMS
// Toggles if UDP checksums should be used or not.
// note Support for UDP checksums is currently not included in uIP,
// so this option has no function.
#define UIP_UDP_CHECKSUMS UIP_CONF_UDP_CHECKSUMS
#else
#define UIP_UDP_CHECKSUMS 0
#endif

//------------------------------------------------------------------------
#ifdef UIP_CONF_UDP_CONNS
// The maximum amount of concurrent UDP connections.
#define UIP_UDP_CONNS UIP_CONF_UDP_CONNS
#else
#define UIP_UDP_CONNS    10
#endif
//
//
//------------------------------------------------------------------------
// Determines if support for opening connections from uIP should be
// compiled in.
// If the applications that are running on top of uIP for this project
// do not need to open outgoing TCP connections, this configration
// option can be turned off to reduce the code size of uIP.
#define UIP_ACTIVE_OPEN 1
//------------------------------------------------------------------------
#ifndef UIP_CONF_MAX_CONNECTIONS
// The maximum number of simultaneously open TCP connections.
// Since the TCP connections are statically allocated, turning this
// configuration knob down results in less RAM used. Each TCP
// connection requires approximatly 30 bytes of memory.

#define UIP_CONNS       10
#else
#define UIP_CONNS UIP_CONF_MAX_CONNECTIONS
#endif
//
//------------------------------------------------------------------------
#ifndef UIP_CONF_MAX_LISTENPORTS
// The maximum number of simultaneously listening TCP ports.
// Each listening TCP port requires 2 bytes of memory.
#define UIP_LISTENPORTS 20
#else /* UIP_CONF_MAX_LISTENPORTS */
#define UIP_LISTENPORTS UIP_CONF_MAX_LISTENPORTS
#endif /* UIP_CONF_MAX_LISTENPORTS */

//------------------------------------------------------------------------
//Determines if support for TCP urgent data notification should be
// compiled in.
// Urgent data (out-of-band data) is a rarely used TCP feature that
// very seldom would be required.

#define UIP_URGDATA      0
//------------------------------------------------------------------------
 // The initial retransmission timeout counted in timer pulses.
 // This should not be changed.
#define UIP_RTO         3
//------------------------------------------------------------------------
 // The maximum number of times a segment should be retransmitted
 // before the connection should be aborted.
 // This should not be changed.
#define UIP_MAXRTX      8
//------------------------------------------------------------------------
// The maximum number of times a SYN segment should be retransmitted
 // before a connection request should be deemed to have been
 // unsuccessful.
 // This should not need to be changed.
//
#define UIP_MAXSYNRTX      5
//------------------------------------------------------------------------
//
// The TCP maximum segment size.
// This is should not be to set to more than
// UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN.
#define UIP_TCP_MSS     (UIP_BUFSIZE - UIP_LLH_LEN - UIP_TCPIP_HLEN)
//------------------------------------------------------------------------
#ifndef UIP_CONF_RECEIVE_WINDOW
//The size of the advertised receiver's window.
// Should be set low (i.e., to the size of the uip_buf buffer) is the
// application is slow to process incoming data, or high (32768 bytes)
// if the application processes data quickly.

#define UIP_RECEIVE_WINDOW UIP_TCP_MSS
#else
#define UIP_RECEIVE_WINDOW UIP_CONF_RECEIVE_WINDOW
#endif
//------------------------------------------------------------------------
//How long a connection should stay in the TIME_WAIT state.
//This configiration option has no real implication, and it should be
//left untouched.
#define UIP_TIME_WAIT_TIMEOUT 120
//------------------------------------------------------------------------
#ifdef UIP_CONF_ARPTAB_SIZE
// The size of the ARP table.
// This option should be set to a larger value if this uIP node will
// have many connections from the local network.
#define UIP_ARPTAB_SIZE UIP_CONF_ARPTAB_SIZE
#else
#define UIP_ARPTAB_SIZE 8
#endif

//-------------------------------------------------------------------------
// The maxium age of ARP table entries measured in 10ths of seconds.
// An UIP_ARP_MAXAGE of 120 corresponds to 20 minutes (BSD
// default).
#define UIP_ARP_MAXAGE 120
//-------------------------------------------------------------------------
#ifndef UIP_CONF_BUFFER_SIZE
// The size of the IP packet buffer.
// The uIP packet buffer should not be smaller than 60 bytes, and does
// not need to be larger than 1500 bytes. Lower size results in lower
// TCP throughput, larger size results in higher TCP throughput.
#define UIP_BUFSIZE     1520
#else /* UIP_CONF_BUFFER_SIZE */
#define UIP_BUFSIZE UIP_CONF_BUFFER_SIZE
#endif /* UIP_CONF_BUFFER_SIZE */


/**
 * Determines if statistics support should be compiled in.
 *
 * The statistics is useful for debugging and to show the user.
 *
 * \hideinitializer
 */
#ifndef UIP_CONF_STATISTICS
#define UIP_STATISTICS  0
#else /* UIP_CONF_STATISTICS */
#define UIP_STATISTICS UIP_CONF_STATISTICS
#endif /* UIP_CONF_STATISTICS */

/**
 * Determines if logging of certain events should be compiled in.
 *
 * This is useful mostly for debugging. The function uip_log()
 * must be implemented to suit the architecture of the project, if
 * logging is turned on.
 *
 * \hideinitializer
 */
#ifndef UIP_CONF_LOGGING
#define UIP_LOGGING     0
#else /* UIP_CONF_LOGGING */
#define UIP_LOGGING     UIP_CONF_LOGGING
#endif /* UIP_CONF_LOGGING */



#ifndef UIP_CONF_BROADCAST
// Broadcast support.
// This flag configures IP broadcast support. This is useful only
// together with UDP.
//
#define UIP_BROADCAST 0
#else /* UIP_CONF_BROADCAST */
#define UIP_BROADCAST UIP_CONF_BROADCAST
#endif /* UIP_CONF_BROADCAST */

/**
 * Print out a uIP log message.
 *
 * This function must be implemented by the module that uses uIP, and
 * is called by uIP whenever a log message is generated.
 */
void uip_log(char *msg);


#ifdef UIP_CONF_LLH_LEN
// The link level header length.
// This is the offset into the uip_buf where the IP header can be
// found. For Ethernet, this should be set to 14. For SLIP, this
// should be set to 0.
//
#define UIP_LLH_LEN UIP_CONF_LLH_LEN
#else /* UIP_CONF_LLH_LEN */
#define UIP_LLH_LEN     14
#endif /* UIP_CONF_LLH_LEN */

//-------------------------------------------------------------------
#ifdef UIP_CONF_BYTE_ORDER
//name CPU architecture configuration
// The CPU architecture configuration is where the endianess of the
// CPU on which uIP is to be run is specified. Most CPUs today are
// little endian, and the most notable exception are the Motorolas
// which are big endian. The BYTE_ORDER macro should be changed to
// reflect the CPU architecture on which uIP is to be run.
//
// The byte order of the CPU architecture on which uIP is to be run.

// This option can be either BIG_ENDIAN (Motorola byte order) or
// LITTLE_ENDIAN (Intel byte order).
#define UIP_BYTE_ORDER     UIP_CONF_BYTE_ORDER
#else /* UIP_CONF_BYTE_ORDER */
#define UIP_BYTE_ORDER     UIP_LITTLE_ENDIAN
#endif /* UIP_CONF_BYTE_ORDER */
//
//

#define UIP_APPCALL(...)

//typedef uip_tcp_appstate_t

//typedef uip_udp_appstate_t

#endif
