#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include "request.h"
#include "global.h"

class HttpResponse {
private:

  HttpRequest     *request;
  StrMap           headers;

  int              statusCode;
  const char      *statusMessage;

  int              bodyLength;
  int              bodyCapacity;
  char            *body;

public:

  HttpResponse(HttpRequest*);
  ~HttpResponse();

  void setStatus(int, const char*);
  void setHeader(const char*, const char*);

  void write(const char*, size_t);
  void send();
};

#endif
