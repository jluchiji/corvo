#include <stdio.h>

#include "process/puppet.h"
#include "http/headers.h"
#include "http/server.h"
#include "http/mime.h"
#include "trace.h"

#include "middleware/redirect.h"
#include "middleware/loadmod.h"
#include "middleware/cgi.h"

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

  Puppet::init(argv[0]);

  HttpServer *server = new HttpServer(POOL);

  server -> route("GET", "*",         new Serve("http-root-dir/htdocs"));
  server -> route("GET", "/cgi-bin/*",new CGI("http-root-dir"));
  server -> route("*",   "/cgi-bin/*.so", new LoadMod("http-root-dir"));
  server -> route("*",   "!!error/*", new Error());

  server -> listen(9090);

}
