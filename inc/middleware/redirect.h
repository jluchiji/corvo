#ifndef _REDIRECT_H_
#define _REDIRECT_H_

#include "base.h"
#include "http/request.h"
#include "http/response.h"

class Redirect : public HttpMiddleware {
private:
  const char    *path;

public:

  Redirect(const char*);
  ~Redirect();

  void handle(HttpRequest*, HttpResponse*);

};

#endif
