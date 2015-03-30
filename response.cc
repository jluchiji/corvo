#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <cmath>

#include "response.h"
#include "request.h"
#include "headers.h"
#include "global.h"

HttpResponse::HttpResponse(HttpRequest *request) {
  this -> request = request;
  this -> setStatus(RES_501);

  bodyLength   = 0;
  bodyCapacity = SZ_LINE_BUFFER;
  body         = new char[SZ_LINE_BUFFER];
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
HttpResponse::write(const char *buffer, size_t count) {
  /* Ensure that body has sufficient capacity */
  if (bodyCapacity - bodyLength < count) {
    int sz = SZ_LINE_BUFFER;
    while (sz <= bodyLength + count) { sz *= 2; }
    body = (char*) realloc(body, sz);
    bodyCapacity = sz;
  }

  /* Copy over the buffer */
  memcpy(body + bodyLength, buffer, count);
  bodyLength += count;
}

void
HttpResponse::send() {
  int sock = request -> sock;

  /* Write the metadata */
  dprintf(sock, "HTTP/1.1 %d %s\r\n", statusCode, statusMessage);

  /* Print headers */
  for (StrMap::iterator it = headers.begin(); it != headers.end(); ++it) {
    dprintf(sock, "%s: %s\r\n", it -> first, it -> second);
  }

  /* Add Content-Length if necessary */
  if (bodyLength > 0) {
    dprintf(sock, "Content-Length: %d\r\n", bodyLength);
  }

  /* Add the empty line */
  dprintf(sock, "\r\n");

  /* Add body, if appropriate */
  ::write(sock, body, bodyLength);

  /* Shutdown and close the socket */
  shutdown(sock, SHUT_WR);
  close(sock);
}
