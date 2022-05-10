#ifndef KOLIBRI_NETWORK_H
#define KOLIBRI_NETWORK_H

#include <sys/socket.h>
#include <sys/ksys.h>

#define EAI_ADDRFAMILY 1
#define EAI_AGAIN      2
#define EAI_BADFLAGS   3
#define EAI_FAIL       4
#define EAI_FAMILY     5
#define EAI_MEMORY     6
#define EAI_NONAME     8
#define EAI_SERVICE    9
#define EAI_SOCKTYPE   10
#define EAI_BADHINTS   12
#define EAI_PROTOCOL   13
#define EAI_OVERFLOW   14

// Flags for addrinfo
#define AI_PASSIVE     1
#define AI_CANONNAME   2
#define AI_NUMERICHOST 4
#define AI_NUMERICSERV 8
#define AI_ADDRCONFIG  0x400

#pragma pack(push, 1)
struct ARP_entry{
unsigned int IP;
unsigned char MAC[6];
unsigned short status;
unsigned short TTL;
};
#pragma pack(pop)

#pragma pack(push, 1)  
struct addrinfo {
    int     ai_flags;
    int     ai_family;
    int     ai_socktype;
    int     ai_protocol;
    int     ai_addrlen;
    char   *ai_canonname;
    struct sockaddr *ai_addr;
    struct addrinfo *ai_next;
};  
#pragma pack(pop)

DLLAPI int __stdcall inet_addr(const char* hostname);
DLLAPI char* __stdcall inet_ntoa(int ip_addr);
DLLAPI int __stdcall getaddrinfo(const char* hostname, const char* servname, const struct addrinfo* hints, struct addrinfo** res);
DLLAPI void __stdcall freeaddrinfo(struct addrinfo* ai);

#endif
