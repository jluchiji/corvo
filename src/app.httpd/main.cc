#include <stdio.h>

#include "http/headers.h"
#include "http/server.h"
#include "trace.h"

#include "error.h"
#include "serve.h"

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

  HttpServer *server = new HttpServer(POOL);

  server -> route("GET", "*",         new StaticFileServer("http-root-dir"));
  server -> route("*",   "/",         new ErrorHandler(RES_403));
  server -> route("*",   "!!error/*", new ErrorHandler());
  server -> listen(9090);

}
