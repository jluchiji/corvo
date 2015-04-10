#ifndef _CGI_H_
#define _CGI_H_

#include "io/path.h"
#include "trace.h"
#include "base.h"

class CGI : public HttpMiddleware {
private:
  Path     *root;

public:

  CGI(const char*);
  ~CGI();

  void handle(HttpRequest*, HttpResponse*);

};

#endif
