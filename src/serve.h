#ifndef _SERVE_H_
#define _SERVE_H_

#include "include/corvo/handler.h"
#include "include/corvo/request.h"
#include "include/corvo/response.h"

#define HTTP_ROOT "http-root-dir"

class StaticFileServer : public HttpHandler {

public:

  void handle(HttpRequest*, HttpResponse*);


};

#endif
