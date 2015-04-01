#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include "include/transpose/buffer.h"
#include "include/corvo/request.h"
#include "include/global.h"

class HttpResponse {
private:

  StrMap           headers;

  Buffer          *buffer;

  bool             sent;

public:
  HttpRequest     *request;
  int              statusCode;
  const char      *statusMessage;

  HttpResponse(HttpRequest*);
  ~HttpResponse();

  void setStatus(int, const char*);
  void setHeader(const char*, const char*);

  void write(const void*, size_t);
  void write(const char*);

  void send();
};

#endif
