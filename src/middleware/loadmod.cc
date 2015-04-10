#include <dlfcn.h>
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

  /* Load the  */

  response -> setStatus(RES_501);
  response -> write("Hello from LoadMod: ");
  response -> write(path -> str());
}
