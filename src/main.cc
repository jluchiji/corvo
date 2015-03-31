#include <stdio.h>

#include "include/corvo/request.h"
#include "include/corvo/response.h"
#include "include/corvo/server.h"
#include "include/corvo/headers.h"
#include "include/trace.h"

#include "embed/405.h"

void serve(HttpRequest*, HttpResponse*);
void error(HttpRequest*, HttpResponse*);

int main(int argc, char *argv[]) {

  printf(LGRAY("CORVO - The HTTP Server\n"));
  printf(LGRAY("Copyright (C) Denis Luchkin-Zhou (build %s)\n"), __TIMESTAMP__);

  // Print enabled debug levels
  DBG_ERR_N(DGRAY("DEBUG FLAGS : "));
  DBG_ERR_N(LRED("ERROR"));
  DBG_ERR_N(" ");
  DBG_WARN_N(YELLOW("WARNING"));
  DBG_WARN_N(" ");
  DBG_INFO_N(LBLUE("INFO"));
  DBG_INFO_N(" ");
  DBG_VERBOSE_N(DGRAY("VERBOSE"));
  printf("\n\n");

  HttpServer *server = new HttpServer(NONE);

  server -> route("POST", "/cgi-bin/*", &serve);
  server -> route("*", "!!error/405", &error);
  server -> listen(8080);

}

void serve(HttpRequest *request, HttpResponse *response) {
  response -> setStatus(RES_200);
  response -> setHeader(RES_POW);
  response -> setHeader("Date", "Sat, 28 Mar 2015 01:30:15 GMT");
  response -> setHeader("Content-Type", "text/html");
  response -> write("Hello World!", 12);
}



void error(HttpRequest *request, HttpResponse *response) {
  response -> write(__405_res, __405_res_len);
}
