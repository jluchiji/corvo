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

#include "include/corvo/request.h"
#include "include/corvo/server.h"
#include "include/corvo/util.h"
#include "include/global.h"
#include "include/trace.h"

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
  delete ip;

  if (verb)  { delete verb;  }
  if (path)  { delete path;  }
  if (body)  { delete body;  }
  if (query) { delete query; }

}

int HttpRequest::read() {
  if (read_meta() || read_body()) {
    DBG_ERR("Malformed request detected.\n");
    return 1;
  }
  DBG_VERBOSE("Finished reading request.\n");
  return 0;
}

int  HttpRequest::read_meta() {
  char  buffer[SZ_LINE_BUFFER];
  char *start, *end;

  /* Read metadata */
  Util::readline(sock, buffer, SZ_LINE_BUFFER);

  /* Get the verb */
  start = buffer;
  end   = strstr(start, " ");
  if (!end) { return 1; }
  verb  = strndup(start, end - start);

  /* Get the path */
  start = end + 1;
  end   = EITHER(strstr(start, "?"), strstr(start, " "));
  if (!end) { return 1; }
  path  = strndup(start, end - start);

  /* Get the query string (if present) */
  if (*end == '?') {
    start = end + 1;
    end   = strstr(start, " ");
    if (!end) { return 1; }
    query = strndup(start, end - start);
  } else { query = NULL; }

  /* Read headers */
  while (Util::readline(sock, buffer, SZ_LINE_BUFFER)) {
    char *key   = NULL,
         *value = NULL;

    /* Read the header name */
    start = buffer;
    end   = strstr(start, ":");
    if (!end) { return 1; }
    key   = strndup(start, end - start);

    /* Skip LWS characters per HTTP/1.1 SPEC section 4.2 */
    end += 1;
    while (LWS(*end)) { end++; }

    /* Read the header value */
    start = end;
    value = strdup(start);

    /* Insert the header into the request object */
    if (key && value) {
      headers[key] = value;
      DBG_VERBOSE("%s: %s\n", key, value);
    }
  }

  /* All good */
  return 0;
}

int  HttpRequest::read_body() {
  char  buffer[SZ_LINE_BUFFER];
  char *start, *end;

  /* Save content length for later use */
  char *strLen      = headers["Content-Length"];
  int contentLength = -1;
  if (strLen) { contentLength = atoi(strLen); }

  /* Read request body */
  if (contentLength > 0) {
    int total = 0;
    std::string *tmp = new std::string();
    while (1) {
      int count = ::read(sock, buffer, SZ_LINE_BUFFER);
      tmp -> append(buffer, MIN(count, contentLength - total));
      total += count;
      if (total >= contentLength) { break; }
    }
    body = strdup(tmp -> c_str());
    delete tmp;
    DBG_VERBOSE("Body:   %s\n", body);
  } else { body = NULL; }

  return 0;
}
