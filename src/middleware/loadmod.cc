
#include <errno.h>
#include <dlfcn.h>
#include <string.h>
#include "http/headers.h"
#include "http/server.h"
#include "io/fileinfo.h"
#include "loadmod.h"

LoadMod::LoadMod(const char *path) {
  this -> root = new Path(path);
}

LoadMod::~LoadMod() {
  delete root;
}

void
LoadMod::handle(HttpRequest *request, HttpResponse *response) {
  Path *path = root -> clone() -> push(request -> path);

  /* Detect file existence */
  FileInfo fi(path);
  if (!fi.exists()) {
    response -> setStatus(RES_404);
    request -> server -> panic(request, response);
    return;
  }

  /* Load the module */
  char *strPath = path -> str();
  void *module = dlopen(strPath, RTLD_LAZY);
  delete strPath;

  /* Detect errors */
  if (!module) {
    response -> setStatus(RES_500);
    request -> server -> panic(request, response);
    return;
  }

  /* Find the handler function */
  CgiInterface handler = (CgiInterface) dlsym(module, "httprun");

  /* Detect errors */
  if (!handler) {
    DBG_ERR("%s: module has no httprun method.\n", request -> path);
    response -> setStatus(RES_500);
    request -> server -> panic(request, response);
    return;
  }

  /* Send headers */
  response -> setStatus(RES_200);
  response -> sendHeaders();

  /* Hand over the control */
  handler(request -> sock, request -> query);

  /* Close the module handle */
  dlclose(module);

  /* Disable response from sending the response */
  response -> disable();
}
