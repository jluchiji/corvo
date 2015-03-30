#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <string>

#include "request.h"
#include "server.h"
#include "global.h"
#include "trace.h"
#include "util.h"

pthread_mutex_t HttpRequest::mutex;

HttpRequest::HttpRequest(HttpServer* server) {

  Addr_in address;
  int len = sizeof(address);

  this -> server = server;

  pthread_mutex_lock(&HttpRequest::mutex);
  sock = accept(server -> getSocket(), (Addr*)&address, (socklen_t*)&len);
  pthread_mutex_unlock(&HttpRequest::mutex);

  if (sock < 0) { COMPLAIN("accept: %s", strerror(errno)); }

  ip = new Addr_in();
  memcpy(ip, &address, sizeof(address));
}

HttpRequest::~HttpRequest() {
  close(sock);
}

void HttpRequest::read() {
  char buffer[SZ_LINE_BUFFER];

  /* Read metadata */
  Util::readline(sock, buffer, SZ_LINE_BUFFER);
  DBG_INFO("%s\n", buffer);

  /* Read headers */
  while (Util::readline(sock, buffer, SZ_LINE_BUFFER)) {
    DBG_INFO("%s\n", buffer);
  }

  /*  */

  DBG_INFO("Finished reading request.\n")
}
