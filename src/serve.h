#ifndef _SERVE_H_
#define _SERVE_H_

#include "fs/path.h"
#include "fs/fileinfo.h"
#include "corvo/handler.h"
#include "corvo/request.h"
#include "corvo/response.h"

#define HTTP_ROOT "http-root-dir"

class StaticFileServer : public HttpHandler {
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
