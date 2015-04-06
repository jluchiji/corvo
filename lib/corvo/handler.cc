#include "include/corvo/response.h"
#include "include/corvo/headers.h"
#include "include/corvo/request.h"
#include "include/corvo/handler.h"

void
HttpHandler::handle(HttpRequest *request, HttpResponse *response) {
  //response -> setStatus(RES_500);
  response -> write(response -> statusMessage);
}
