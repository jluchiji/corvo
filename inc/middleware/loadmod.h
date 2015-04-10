#ifndef _LOADMOD_H_
#define _LOADMOD_H_

#include "io/path.h"
#include "trace.h"
#include "base.h"

typedef void (*CgiInterface)(int, const char*);

class LoadMod : public HttpMiddleware {
private:
  Path    *root;

public:

  LoadMod(const char*);
  ~LoadMod();

  void handle(HttpRequest*, HttpResponse*);
};

#endif
