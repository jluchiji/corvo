#include <cstdlib>

#include "process/puppet.h"
#include "http/headers.h"
#include "http/server.h"
#include "io/fileinfo.h"
#include "cgi.h"

CGI::CGI(const char *root) {
  this -> root = new Path(root);
}

CGI::~CGI() {
  delete root;
}

void
CGI::handle(HttpRequest *request, HttpResponse *response) {

  Path *path = root -> clone() -> push(request -> path);

  /* Detect file existence */
  FileInfo fi(path);
  if (!fi.exists()) {
    response -> setStatus(RES_404);
    request -> server -> panic(request, response);
    return;
  }

  /* Set environment variables as appropriate */
  setenv("REQUEST_METHOD", request -> verb, 1);
  setenv("QUERY_STRING", request -> query, 1);

  /* Spin up our puppet */
  char *strPath = path -> str();
  Puppet puppet(strPath);
  delete strPath;

  /* Execute the puppet */
  puppet.write(request -> body);
  puppet.run();

  /* Set up and send header information */
  if (!strcmp(request -> verb, "POST")) {
    response -> setStatus(RES_201);
  }
  else { response -> setStatus(RES_200); }

  response -> sendHeaders();

  /* Write puppet output to response socket */
  int sock = request -> sock;

  size_t count;
  char buffer[SZ_LINE_BUFFER];

  while ((count = puppet.read(IO_OUT, buffer, SZ_LINE_BUFFER))) {
    write(sock, buffer, count);
  }

  /* Finish the response process */
  response -> finalize();
}
