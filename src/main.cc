#include <stdio.h>

#include "include/corvo/server.h"
#include "include/trace.h"

#include "src/error.h"
#include "src/serve.h"

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

  server -> route("GET", "/*", &serve);
  //server -> route("*", "!!error/405", &error);
  //server -> route("*", "!!error/404", &error);
  server -> listen(8080);

}
