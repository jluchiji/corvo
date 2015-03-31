#include "src/error.h"
#include "embed/404.h"
#include "embed/405.h"

void error(HttpRequest *request, HttpResponse *response) {
  switch (response -> statusCode) {
    case 404:
      response -> write(__404_res, __404_res_len);
      break;
    case 405:
      response -> write(__405_res, __405_res_len);
      break;
  }

}
