#ifndef _HTTP_SERVE_H_
#define _HTTP_SERVE_H_


#include "middleware/base.h"
#include "response.h"
#include "request.h"
#include "global.h"
#include <list>

#define SZ_ERR_ROUTE  12
#define FMT_ERR_ROUTE "!!error/%d"

/* HTTP Server Operation Mode */
enum HttpServerMode {
  NONE,
  FORK,
  POOL,
  THREAD
};

/* HTTP Request Handler */
typedef struct {
  char             *verb;
  Regex            *regex;
  HttpMiddleware      *handler;
} HttpMiddlewareMacro;
typedef std::list<HttpMiddlewareMacro*> HandlerMap;

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

  void route(const char*, const char*, HttpMiddleware*);

  int  getSocket();
  HttpMiddleware* find_handler(const char*, const char*);

  void redirect(const char*, const char*, HttpRequest*, HttpResponse*);
  void panic(HttpRequest*, HttpResponse*);
};

#endif
