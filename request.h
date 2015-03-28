#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <pthread.h>
#include "global.h"

class HttpServer;

class HttpRequest {
private:
  static pthread_mutex_t mutex;

  int              sock;
  HttpServer      *server;
  Addr_in         *ip;

public:

  HttpRequest(HttpServer*);
  ~HttpRequest();


};

#endif
