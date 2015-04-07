#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <cmath>

#include "transpose/buffer.h"
#include "corvo/response.h"
#include "corvo/request.h"
#include "corvo/headers.h"
#include "global.h"
#include "trace.h"

HttpResponse::HttpResponse(HttpRequest *request) {
  this -> request = request;
  this -> setStatus(RES_501);

  sent    = false;
  buffer  = new Buffer();
}

HttpResponse::~HttpResponse() {

}

void
HttpResponse::setStatus(int code, const char* message) {
  statusCode    = code;
  statusMessage = message;
}

void
HttpResponse::setHeader(const char *name, const char *value) {
  /* Make sure that the value actually has content */
  if (value && strlen(value)) {
    if (headers[name]) { headers[name] = strdup(value); }
    else { headers[strdup(name)] = strdup(value); }
  }
  /* If value is null or empty, remove the header */
  else { headers.erase(name); }
}

void
HttpResponse::write(const void *buffer, size_t count) {
  this -> buffer -> write(buffer, count);
}

void
HttpResponse::write(const char *buffer) {
  write(buffer, strlen(buffer));
}

void
HttpResponse::send() {
  if (sent) { return; }

  int sock = request -> sock;

  /* Write the metadata */
  dprintf(sock, "HTTP/1.1 %d %s\r\n", statusCode, statusMessage);

  /* Print headers */
  for (StrMap::iterator it = headers.begin(); it != headers.end(); ++it) {
    DBG_VERBOSE("%s: %s\n", it -> first, it -> second);
    dprintf(sock, "%s: %s\r\n", it -> first, it -> second);
    DBG_VERBOSE("%s: %s\n", it -> first, it -> second);
  }
  DBG_VERBOSE("SEND RESPONSE\n");

  /* Add Content-Length if necessary */
  if (buffer -> length() > 0) {
    dprintf(sock, "Content-Length: %lu\r\n", buffer -> length());
  }

  /* Add the empty line */
  dprintf(sock, "\r\n");

  /* Add body, if appropriate */
  ::write(sock, buffer -> data(), buffer -> length());

  /* Shutdown and close the socket */
  shutdown(sock, SHUT_WR);
  sent = true;
  close(sock);
}
