#ifndef _LOADMOD_H_
#define _LOADMOD_H_

#include "io/path.h"
#include "base.h"

class LoadMod : public HttpMiddleware {
private:
  Path    *root;

public:

  LoadMod(const char*);
  ~LoadMod();

  void handle(HttpRequest*, HttpResponse*);
};

#endif
