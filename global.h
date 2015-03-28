#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <sys/socket.h>
#include <string.h>
#include <map>

/* String comparator */
struct StringComp :
  public std::binary_function<const char*, const char*, bool> {
    bool operator()(char const *a, char const *b) const {
      return strcasecmp(a, b) < 0;
    }
};

/* Constants */
#define SZ_THREAD_POOL 8
#define SZ_REQ_QUEUE   5
#define DEFAULT_PORT   8080

/* Type Definitions */
typedef struct sockaddr_in Addr_in;
typedef struct sockaddr    Addr;
typedef void* (*ThreadHandler)(void*);
typedef std::pair<const char*, char*> StrPair;
typedef std::map<const char*, char*, StringComp> StrMap;

#endif
