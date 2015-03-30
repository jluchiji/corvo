#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <pthread.h>
#include "global.h"

class HttpServer;

class HttpRequest {
private:
  static pthread_mutex_t mutex;

  int    read_meta();
  int    read_body();

public:

  int              sock;
  HttpServer      *server;
  Addr_in         *ip;

  char            *verb;
  char            *path;
  char            *body;
  char            *query;
  StrMap           headers;

  HttpRequest(HttpServer*);
  ~HttpRequest();

  int    read();

};

#endif
