#ifndef _HANDLER_H_
#define _HANDLER_H_

#include "http/request.h"
#include "http/response.h"

class HttpMiddleware {
public:
  virtual void handle(HttpRequest*, HttpResponse*);
};

#endif
