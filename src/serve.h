#ifndef _SERVE_H_
#define _SERVE_H_

#include "include/corvo/request.h"
#include "include/corvo/response.h"

#define HTTP_ROOT "http-root-dir"

void serve(HttpRequest*, HttpResponse*);

#endif
