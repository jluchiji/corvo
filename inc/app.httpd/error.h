#ifndef _ERROR_H_
#define _ERROR_H_

#include "http/request.h"
#include "http/response.h"
#include "middleware/base.h"

#define ERR_404 \
  "The resource you are looking for is nowhere to be found.<br>" \
  "We dispatched a search party, but don't count on it."

#define ERR_405 \
  "You have reached a dead end.<br>" \
  "Double check that you mapped this route to a handler function."

#define ERR_DEFAULT \
  "Something went terribly wrong :("

class Error : public HttpMiddleware {
private:
  int         statusCode;
  const char *statusMessage;

public:

  Error();
  Error(int, const char*);

  void handle(HttpRequest*, HttpResponse*);
};

#endif
