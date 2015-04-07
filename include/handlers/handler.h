#ifndef _HANDLER_H_
#define _HANDLER_H_

#include "corvo/request.h"
#include "corvo/response.h"

class HttpHandler {
public:
  virtual void handle(HttpRequest*, HttpResponse*);
};

#endif
