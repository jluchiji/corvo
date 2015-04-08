#include "templating/fragment.h"
#include "http/headers.h"
#include "http/server.h"
#include "io/fileinfo.h"
#include "io/buffer.h"
#include "io/path.h"
#include "global.h"
#include "trace.h"
#include "serve.h"

#include "res/direntry.h"
#include "res/bootstrap.h"
#include "res/listdir.h"
#include "res/styles.h"
#include "res/folder.h"
#include "res/file.h"

#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

StaticFileServer::StaticFileServer(const char *path) {
  root = new Path(path);
}

StaticFileServer::~StaticFileServer() {
  delete root;
}

void
StaticFileServer::serve_file(FileInfo *fi, HttpResponse *response) {
  int  fd = fi -> open(O_RDONLY, S_IREAD);
  int  count;
  char buffer[SZ_LINE_BUFFER];
  while ((count = read(fd, buffer, SZ_LINE_BUFFER)) > 0) {
    response -> write(buffer, count);
  }
}

void
StaticFileServer::serve_dir(FileInfo *fi, HttpResponse *response) {
  /* Render Directory Entries */
  Buffer *buffer = new Buffer();
  FileInfo *child;
  while ((child = fi -> readdir())) {
    /* Skip . and .. */
    if (!child -> getName() || !strcmp(child -> getName(), "..")) { continue; }

    /* Create a fragment for the directory entry and set its name */
    Fragment *entry = new Fragment(RES_DIRENTRY);
    entry -> set("ent-name", child -> getName());

    /* Directory */
    if (child -> getType() == FT_DIR) {
      entry -> set("ent-icon", RES_FOLDER);
      Buffer *link = new Buffer();
      link -> write(child -> getName(), strlen(child -> getName()));
      link -> write("/\0", sizeof(char) * 2);
      entry -> set("ent-link", (char*)link -> data());
      delete link;
    }
    /* File */
    else {
      entry -> set("ent-icon", RES_FILE);
      entry -> set("ent-link", child -> getName());
    }





    entry -> render(buffer);
  }

  /* Render Response Page */
  Fragment *fragment = new Fragment(RES_LISTDIR);
  fragment -> set("css-bootstrap", RES_BOOTSTRAP);
  fragment -> set("css-style", RES_STYLES);
  fragment -> set("dir-name", fi -> getName());
  fragment -> set("dir-content", new Fragment(buffer -> data(), buffer -> length()));
  delete buffer;
  buffer = fragment -> render();

  response -> write(buffer -> data(), buffer -> length());
}

void
StaticFileServer::handle(HttpRequest *request, HttpResponse *response) {
  HttpServer *server = request -> server;
  Path *path = root -> clone();
  path -> push(request -> path);

  FileInfo *fi = new FileInfo(path);

  switch (fi -> getType()) {

    case FT_NOENT: {
      response -> setStatus(RES_404);
      server -> panic(request, response);
    } break;

    case FT_FILE: { serve_file(fi, response); } break;

    case FT_DIR: {

      // If this is a directory and path does not have tailing slash,
      // we need to send back a 301.
      if (request -> path[strlen(request -> path) - 1] != '/') {
        response -> setStatus(RES_301);

        Buffer *location = new Buffer();
        location -> write(request -> path, strlen(request -> path));
        location -> write("/\0", sizeof(char) * 2);
        response -> setHeader("Location", (char*)location -> data());
        delete location;

        return;
      }

      /* Handle opendir errors here */
      if (fi -> opendir()) {
        switch (errno) {
          case EACCES:
            response -> setStatus(RES_403);
            break;
          case ENOENT:
            response -> setStatus(RES_404);
            break;
          default:
            response -> setStatus(RES_500);
            break;
        }
        server -> panic(request, response);
        return;
      }

      /* Check if directory contains index.html */
      Path *indexPath = path -> clone() -> pushd("index.html");
      FileInfo *index = new FileInfo(indexPath);
      delete indexPath;
      if (index -> exists()) {
        serve_file(index, response);
        delete index;
        return;
      }
      delete index;

      /* Serve directory */
      serve_dir(fi, response);

    } break;

  }

  response -> setStatus(RES_200);
  delete path;
}
