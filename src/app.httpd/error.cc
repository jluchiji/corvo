#include "error.h"

#include "trace.h"
#include "global.h"
#include "io/buffer.h"
#include "middleware/base.h"
#include "templating/fragment.h"

#include "res/error.h"
#include "res/styles.h"
#include "res/bootstrap.h"

Error::Error() {
  statusCode = -1;
}

Error::Error(int code, const char *message) {
  statusCode    = code;
  statusMessage = message;
}

void
Error::handle(HttpRequest *request, HttpResponse *response) {
  DBG_ERR("%d %s\n", response -> statusCode, response -> statusMessage);

  if (statusCode > 0) { response -> setStatus(statusCode, statusMessage); }

  char code[4];
  snprintf(code, 4, "%d", response -> statusCode);

  Fragment *fragment = new Fragment(RES_ERROR);
  fragment -> set("status-code", code);
  fragment -> set("status-message", response -> statusMessage);
  fragment -> set("css-bootstrap", RES_BOOTSTRAP);
  fragment -> set("css-style", RES_STYLES);

  switch (response -> statusCode) {
    case 404:
      fragment -> set("status-description", ERR_404);
      break;
    case 405:
      fragment -> set("status-description", ERR_405);
      break;
    default:
      fragment -> set("status-description", ERR_DEFAULT);
      break;
  }

  Buffer *buffer = fragment -> render();

  response -> write(buffer -> data(), buffer -> length());
  delete buffer;
  delete fragment;
}
