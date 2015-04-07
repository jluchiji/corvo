#ifndef _SERVE_H_
#define _SERVE_H_

#include "io/path.h"
#include "io/fileinfo.h"
#include "http/request.h"
#include "http/response.h"
#include "middleware/base.h"

#define HTTP_ROOT "http-root-dir"

class StaticFileServer : public HttpMiddleware {
private:
  Path    *root;

  void serve_file(FileInfo*, HttpResponse*);
  void serve_dir(FileInfo*, HttpResponse*);

public:

  StaticFileServer(const char*);
  ~StaticFileServer();

  void handle(HttpRequest*, HttpResponse*);
};

#endif
