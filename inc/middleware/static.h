#ifndef _STATIC_H_
#define _STATIC_H_

#include "io/fileinfo.h"
#include "base.h"

class StaticFile : public HttpMiddleware {
protected:
  Path           *root;

  virtual void serve_file(FileInfo*, HttpResponse*);
  virtual void serve_dir(FileInfo*, HttpResponse*);

  virtual void render_file(FileInfo*, Buffer*);
  virtual void render_dir(FileInfo*, Buffer*, Buffer*);

public:

  StaticFile(const char*);
  ~StaticFile();

  void handle(HttpRequest*, HttpResponse*);
};

#endif
