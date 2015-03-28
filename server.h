#ifndef _HTTP_SERVE_H_
#define _HTTP_SERVE_H_

#include "response.h"
#include "request.h"
#include "global.h"

/* HTTP Server Operation Mode */
enum HttpServerMode {
  NONE,
  FORK,
  POOL,
  THREAD
};

/* HTTP Server Class */
class HttpServer {
private:
  HttpServerMode       mode;
  pthread_t            pool[SZ_THREAD_POOL];
  pthread_mutex_t      mutex;

  int                  sock;

  static void pool_handler(HttpServer*);
  static void handle(HttpRequest*);

public:
  HttpServer(HttpServerMode);
  ~HttpServer();

  void listen();
  void listen(int);

  int  getSocket();
};

#endif
