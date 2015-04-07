#ifndef _SERVE_H_
#define _SERVE_H_

#include "include/fs/path.h"
#include "include/fs/fileinfo.h"
#include "include/corvo/handler.h"
#include "include/corvo/request.h"
#include "include/corvo/response.h"

#define HTTP_ROOT "http-root-dir"

class StaticFileServer : public HttpHandler {
private:
  Path    *root;

  void serve_file(FileInfo*, HttpResponse*);

public:

  StaticFileServer(const char*);
  ~StaticFileServer();

  void handle(HttpRequest*, HttpResponse*);
};

#endif
