#ifndef _HTTP_SERVE_H_
#define _HTTP_SERVE_H_

#include "include/corvo/response.h"
#include "include/corvo/request.h"
#include "include/global.h"
#include <vector>

/* HTTP Server Operation Mode */
enum HttpServerMode {
  NONE,
  FORK,
  POOL,
  THREAD
};

/* HTTP Request Handler Function */
typedef void (*HttpHandlerFunc)(HttpRequest*, HttpResponse*);

/* HTTP Request Handler */
typedef struct {
  char             *verb;
  Regex            *regex;
  HttpHandlerFunc   handler;
} HttpHandler;
typedef std::vector<HttpHandler*> HandlerMap;

/* HTTP Server Class */
class HttpServer {
private:
  HttpServerMode       mode;
  pthread_t            pool[SZ_THREAD_POOL];
  pthread_mutex_t      mutex;

  int                  sock;

  HandlerMap           handlers;

  static void pool_handler(HttpServer*);
  static void handle(HttpRequest*);

public:
       HttpServer(HttpServerMode);
       ~HttpServer();

  void listen();
  void listen(int);

  void route(const char*, const char*, HttpHandlerFunc);

  int  getSocket();


  static
  void serve(HttpRequest*, HttpResponse*);
};

#endif
