#include "corvo/response.h"
#include "corvo/headers.h"
#include "corvo/request.h"
#include "corvo/handler.h"

void
HttpHandler::handle(HttpRequest *request, HttpResponse *response) {
  //response -> setStatus(RES_500);
  response -> write(response -> statusMessage);
}
