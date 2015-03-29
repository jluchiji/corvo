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
  int current, count;

  /* Read the verb and path */
  verb = readUntil(' ');
  path = readUntil(' ');

  /* Read the protocol and discard it */
  readUntil('\n');

  DBG_INFO("%s %s HTTP/1.1\n", verb, path);


  DBG_INFO("Finished reading request.\n")
}

char* HttpRequest::readUntil(char c) {
  int count;
  char current;
  std::string *content = new std::string();

  while ((count = ::read(sock, &current, sizeof(current))) > 0) {
    if (current == c) break;
    content -> append(&current, 1);
  }

  char *result = strdup(content -> c_str());
  delete content;
  return result;
}
