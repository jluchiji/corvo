#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>

#include "response.h"
#include "request.h"
#include "server.h"
#include "global.h"
#include "trace.h"

HttpServer::HttpServer(HttpServerMode mode) {
  this -> mode = mode;
}

HttpServer::~HttpServer() {

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
  DBG_PRINT("Server ready.\n");

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
        pthread_create(&pool[i], NULL, (ThreadHandler)&pool_handler, (void*)(intptr_t)sock);
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

void HttpServer::pool_handler(HttpServer* server) {
  while (1) {
    HttpRequest *req = new HttpRequest(server);
    handle(req);
    delete req;
  }
}

void HttpServer::handle(HttpRequest *req) {
  DBG_INFO("REQUEST RECEIVED\n");
}

int HttpServer::getSocket() {
  return sock;
}