#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>

#include "middleware/base.h"
#include "util/toregex.h"
#include "response.h"
#include "request.h"
#include "headers.h"
#include "server.h"
#include "global.h"
#include "trace.h"

HttpServer::HttpServer(HttpServerMode mode) {
  this -> mode = mode;
  this -> route("*", "!!error/*", new HttpMiddleware());
}

HttpServer::~HttpServer() {

}

void HttpServer::listen() {
  this -> listen(DEFAULT_PORT);
}

void HttpServer::listen(int port) {
  /* Server Address */
  Addr_in serverAddress;
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family      = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port        = htons((unsigned short)port);

  /* Create Listening Socket */
  sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    COMPLAIN("socket: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  /* Reuse Local IP Addresses */
  int value = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&value, sizeof(int))) {
    COMPLAIN("setsocketopt: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  /* Bind socket to address */
  if (bind(sock, (Addr*)&serverAddress, sizeof(serverAddress))) {
    COMPLAIN("bind: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  /* Listen for requests */
  if (::listen(sock, SZ_REQ_QUEUE)) {
    COMPLAIN("listen: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  /* Ready */
  DBG_PRINT("Server listening on 127.0.0.1:%d\n", port);

  /* Start accepting connections */
  switch (mode) {

    /* No concurrency */
    case NONE: while (1) {
      HttpRequest *req  = new HttpRequest(this);
      handle(req);
    }

    /* Fork new process */
    case FORK: while (1) {
      HttpRequest *req = new HttpRequest(this);
      if (!fork()) { handle(req); exit(0); }
    }

    /* Thread pool */
    case POOL: while (1) {
      for (int i = 0; i < SZ_THREAD_POOL; i++) {
        pthread_create(&pool[i], NULL, (ThreadHandler)&pool_handler, (void*)this);
      }
      pool_handler(this);
    }

    /* Create new thread */
    case THREAD: while (1) {
      HttpRequest *req = new HttpRequest(this);

      pthread_t thread;
      pthread_create(&thread, NULL, (ThreadHandler)&handle, (void*)req);
    }

  }

}

int HttpServer::getSocket() {
  return sock;
}

void HttpServer::pool_handler(HttpServer* server) {
  while (1) {
    HttpRequest *req = new HttpRequest(server);
    handle(req);
  }
}

HttpMiddleware*
HttpServer::find_handler(const char *verb, const char *path) {
  HttpMiddleware *handler = NULL;
  HandlerMap::iterator it = handlers.begin();
  for (; it !=  handlers.end(); ++it) {
    /* Match path regex */
    regmatch_t m;
    if (!regexec((*it) -> regex, path, 1, &m, 0) &&
       (!strcmp((*it) -> verb, "*") || strstr((*it) -> verb, verb))) {
      handler = (*it) -> handler;
      break;
    }
  }
  return handler;
}

void
HttpServer::redirect(const char   *verb,
                     const char   *url,
                     HttpRequest  *request,
                     HttpResponse *response) {
  HttpMiddleware *handler = this -> find_handler(verb, url);
  handler -> handle(request, response);
}

void
HttpServer::panic(HttpRequest* request, HttpResponse* response) {

  char *errorRoute = new char[SZ_ERR_ROUTE];
  snprintf(errorRoute, SZ_ERR_ROUTE, FMT_ERR_ROUTE, response -> statusCode);
  HttpMiddleware* handler = find_handler("*", errorRoute);
  handler -> handle(request, response);
  delete errorRoute;
}

void HttpServer::handle(HttpRequest *request) {

  /* Create the response object */
  HttpServer   *server   = request -> server;
  HttpResponse *response = new HttpResponse(request);
  response -> setHeader(RES_POW); // Default header

  /* Read request data and look for errors */
  if (request -> read()) {
    // XXX Does this cause problems..?
    //response -> setStatus(RES_400);
    //response -> send();
    //delete request;
    //delete response;
    return;
  }

  /* Get the client IP address for logging purposes */
  Addr_in *ip = request -> ip;
  char *buffer = inet_ntoa(ip -> sin_addr);
  DBG_INFO("[%s] %s %s\n", buffer, request -> verb, request -> path);

  /* Find the first matching route handler */
  HttpMiddleware *handler = request -> server -> find_handler(request -> verb, request -> path);

  /* If handler was not found...that is a 405 */
  if (!handler) {
    response -> setStatus(RES_405);
    request -> server -> redirect("*", "!!error/405", request, response);
  } else {
    handler -> handle(request, response);
  }

  response -> send();
  delete request;
  delete response;
}

void HttpServer::route(const char      *verb,
                       const char      *path,
                       HttpMiddleware     *handler) {
  /*  */
  HttpMiddlewareMacro *mapping = new HttpMiddlewareMacro();
  mapping -> verb    = strdup(verb);
  mapping -> regex   = toRegex(path);
  mapping -> handler = handler;

  handlers.push_front(mapping);
}
