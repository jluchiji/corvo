#include "redirect.h"
#include "http/headers.h"

Redirect::Redirect(const char *path) {
  this -> path = strdup(path);
}

Redirect::~Redirect() {
  delete path;
}

void
Redirect::handle(HttpRequest *request, HttpResponse *response) {
  response -> setStatus(RES_302);
  response -> setHeader("Location", path);
}
