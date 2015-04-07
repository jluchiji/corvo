#include "http/response.h"
#include "http/headers.h"
#include "http/request.h"
#include "base.h"

void
HttpMiddleware::handle(HttpRequest *request, HttpResponse *response) {
  //response -> setStatus(RES_500);
  response -> write(response -> statusMessage);
}
