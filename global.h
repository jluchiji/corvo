#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <sys/socket.h>

/* Constants */
#define SZ_THREAD_POOL 8
#define SZ_REQ_QUEUE   5

/* Type Definitions */
typedef struct sockaddr_in Addr_in;
typedef struct sockaddr    Addr;
typedef void* (*ThreadHandler)(void*);

#endif
