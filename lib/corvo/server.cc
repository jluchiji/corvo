#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>

#include "include/corvo/response.h"
#include "include/corvo/request.h"
#include "include/corvo/headers.h"
#include "include/corvo/server.h"
#include "include/corvo/util.h"
#include "include/global.h"
#include "include/trace.h"

HttpServer::HttpServer(HttpServerMode mode) {
  this -> mode = mode;
  this -> route("*", "!!error/*", HttpServer::error);
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

HttpHandlerFunc
HttpServer::find_handler(const char *verb, const char *path) {
  HttpHandlerFunc handler = NULL;
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

void HttpServer::handle(HttpRequest *request) {

  /* Create the response object */
  HttpServer   *server   = request -> server;
  HttpResponse *response = new HttpResponse(request);
  response -> setHeader(RES_POW); // Default header

  /* Read request data and look for errors */
  if (request -> read()) {
    response -> setStatus(RES_400);
    response -> send();
    //delete request;
    //delete response;
    return;
  }

  /* Get the client IP address for logging purposes */
  Addr_in *ip = request -> ip;
  char *buffer = inet_ntoa(ip -> sin_addr);
  DBG_INFO("[%s] %s %s\n", buffer, request -> verb, request -> path);

  /* Find the first matching route handler */
  HttpHandlerFunc handler = request -> server -> find_handler(request -> verb, request -> path);

  /* If handler was not found...that is a 405 */
  if (!handler) {
    response -> setStatus(RES_405);
    HttpHandlerFunc errHandler = request -> server -> find_handler("*", "!!error/405");
    (*errHandler)(request, response);
  } else {
    (*handler)(request, response);
  }

  response -> send();
  delete request;
  delete response;
}

void HttpServer::route(const char      *verb,
                       const char      *path,
                       HttpHandlerFunc  handler) {
  /*  */
  HttpHandler *mapping = new HttpHandler();
  mapping -> verb    = strdup(verb);
  mapping -> regex   = Util::toRegex(path);
  mapping -> handler = handler;

  handlers.push_front(mapping);
}

void HttpServer::error(HttpRequest  *request,
                       HttpResponse *response) {
  response -> write(response -> statusMessage);
}
