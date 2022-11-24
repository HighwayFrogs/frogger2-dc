/************************************************************************/
/*    AVE-TCP Socket Library     socket.h            92/04/15          */
/*       Copyright (C) 1992 ACCESS CO.,LTD.                             */
/************************************************************************/
/*
 * Definition related to sockets: types, address families, options.
 */

#ifdef __cplusplus /* Support for C++ */
extern "C" {
#endif /* __cplusplus */

/*
 * Types
 */
#define SOCK_STREAM 1               /* stream socket */
#define SOCK_DGRAM  2               /* datagram socket */
#define SOCK_RAW    3               /* raw-protocol interface */
#define SOCK_RDM    4               /* reliably-delivered message */
#define SOCK_SEQPACKET  5               /* sequenced packet stream */

/*
 * Option flags per\socket.
 */
#define SO_DEBUG    0x0001          /* turn on debugging info recording */
#define SO_ACCEPTCONN   0x0002          /* socket has had listen() */
#define SO_REUSEADDR    0x0004          /* allow local address reuse */
#define SO_KEEPALIVE    0x0008          /* keep connections alive */
#define SO_DONTROUTE    0x0010          /* just use interface addresses */
#define SO_BROADCAST    0x0020          /* permit sending of broadcast msgs */
#define SO_USELOOPBACK  0x0040          /* bypass hardware when possible */
#define SO_LINGER   0x0080          /* linger on close if data present */
#define SO_OOBINLINE    0x0100          /* leave received OOB data in line */
#define SO_NOBLK    0x0200          /* no blocking mode */

/*
 * N.B.: The following definition is present only for campatibility
 * with release 3.0.  It will disappear in later releases.
 */
#define SO_DONTLINGER   (~SO_LINGER)    /* ~SO_LINGER */

/*
 * Additional options, not kept in so_options.
 */
#define SO_SNDBUF   0x1001      /* send buffers size */
#define SO_RCVBUF   0x1002      /* recieve buffer size */
#define SO_SNDLOWAT 0x1003      /* send low-water mark */
#define SO_RCVLOWAT 0x1004      /* receive low-water mark */
#define SO_SNDTOMEO 0x1005      /* send timeout */
#define SO_RCVTIMEO 0x1006      /* receive timeout */
#define SO_ERROR    0x1007      /* get error status and clear */
#define SO_TYPE     0x1008      /* get socket types */

/*
 * Structure used for manipulating linger option.
 */
struct  linger {
    int l_onoff;        /* option on/off */
    int l_linger;       /* linger time */
};

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define SOL_SOCKET  0xffff      /* options for socket level */

/*
 * Address families.
 */
#define AF_UNSPEC   0       /* unspecified */
#define AF_UNIX     1       /* local to host (pipes, portals) */
#define AF_INET     2       /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK  3       /* arpanet imp addresses */
#define AF_PUP      4       /* pup protocols: e.q. BSP */
#define AF_CHAOS    5       /* mit CHAOS protocols */
#define AF_NS       6       /* XEROX NS protocols */
#define AF_NBS      7       /* nbs protocols */
#define AF_ECMA     8       /* european computer manufacturers */
#define AF_DATAKIT  9       /* datakit protocols */
#define AF_CCITT    10      /* CCITT protocols, X.25 etc */
#define AF_SNA      11      /* IBM SNA */
#define AF_DECnet   12      /* DECnet */
#define AF_DLI      13      /* Direct data link interface */
#define AF_LAT      14      /* LAT */
#define AF_HYLINK   15      /* NSC Hyperchannel */
#define AF_APPLETALK    16      /* Apple Talk */

#define AF_NIT      17      /* Network Interface Tap */
#define AF_802      18      /* IEEE 802.2, also ISO 8802 */
#define AF_OSI      19      /* umbrella for all families used
                     * by OSI (e.g. protosw lookup) */
#define AF_X25      20      /* CCITT X.25 in particular */
#define AF_OSINET   21      /* AFI = 47, IDI = 4 */
#define AF_GOSIP    22      /* U.S. Government OSI */

#define AF_MAX      21

/*
 * Structure used by kernel to store most
 * addresses.
 */
struct sockaddr {
    u_short sa_family;      /* address family */
    char    sa_data[14];        /* up to 14 bytes of direct address */
};

/*
 * Structure used by kernel to pass protocol
 * information in raw sockets.
 */
struct sockproto {
    u_short ap_family;      /* address family */
    u_short sp_protocol;        /* protocol */
};

/*
 * Protocol families, same as address families for now.
 */
#define PF_UNSPEC   AF_UNSPEC
#define PF_UNIX     AF_UNIX
#define PF_INET     AF_INET
#define PF_IMPLINK  AF_IMPLINK
#define PF_PUP      AF_PUP
#define PF_CHAOS    AF_CHAOS
#define PF_NS       AF_NS
#define PF_NBS      AF_NBS
#define PF_ECMA     AF_ECMA
#define PF_DATAKIT  AF_DATAKIT
#define PF_CCITT    AF_CCITT
#define PF_SNA      AF_SNA
#define PF_DECnet   AF_DECnet
#define PF_DLI      AF_DLI
#define PF_LAT      AF_LAT
#define PF_HYLINK   AF_HYLINK
#define PF_APPLETALK    AF_APPLETALK
#define PF_NIT      AF_NIT
#define PF_802      AF_802
#define PF_OSI      AF_OSI
#define PF_X25      AF_X25
#define PF_OSINET   AF_OSINET
#define PF_GOSIP    AF_GOSIP

#define PF_MAX      AF_MAX

/*
 * Maximum queue length specifiable by listen.
 */
#define SOMAXCONN   5

/*
 * Message header for recvmsg and sendmsg calls.
 */
struct  msghdr {
    caddr_t msg_name;       /* optional address */
    int msg_namelen;        /* size of address */
    struct  iovec *msg_lov;     /* scatter/gather array */
    int msg_iovlen;     /* # elements in msg_iov */
    caddr_t msg_accrights;      /* access rights sent/received */
    int msg_accrightslen;
};

#define MSG_OOB     0x01        /* process out-of-band data */
#define MSG_PEEK    0x02        /* peek at incoming message */
#define MSG_DONTROUTE   0x04        /* send without using routing tables */

#define MSG_MAXIOVLEN   16

/***   Function Definition   ***/

#include <in.h>
#include <netdb.h>
#include <time.h>
#include <types.h>


int accept(int, struct sockaddr *, int *);
int bind(int, struct sockaddr *, int );
int connect(int, struct sockaddr *, int );
int getpeername(int, struct sockaddr *, int * );
int getsockname(int, struct sockaddr *, int * );
int getsockopt(int, int, int, char *, int * );
int setsockopt(int, int, int, char *, int );
int listen(int, int );
int recv(int, char *, int, int );
int recvfrom(int, char *, int, int, struct sockaddr *, int *);
int select(int, fd_set *, fd_set *, fd_set *, struct timeval * );
int send(int, char *, int, int );
int sendto(int, char *, int, int, struct sockaddr *, int );
int shutdown(int, int );
int socket(int, int, int );
int sock_close(int );
int sock_closeall( void );
int sock_read( int, char *, int );
int sock_write(int, char *, int );
struct  hostent *gethostbyname( char * );
u_long  htonl(u_long);
u_short htons(u_short);
u_long  ntohl(u_long);
u_short ntohs(u_short);

#ifdef __cplusplus
} /* End of extern "C" { */
#endif /* __cplusplus */
