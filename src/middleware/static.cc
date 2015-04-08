#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "templating/fragment.h"
#include "http/headers.h"
#include "http/server.h"
#include "http/mime.h"
#include "io/fileinfo.h"
#include "io/buffer.h"
#include "io/path.h"
#include "global.h"
#include "trace.h"
#include "static.h"

#define DEF_DIRITEM "<li><a href=\"{{ent-link}}\">{{ent-name}}</a></li>"
#define DEF_DIRLIST "<!doctype html><html><head><title>{{dir-name}}</title></head>"\
                    "<body><ul>{{dir-content}}</ul></body></html>"


Static::Static(const char *path) {
  root = new Path(path);
}

Static::~Static() {
  delete root;
}

void
Static::serve_file(FileInfo *fi, HttpResponse *response) {
  /* Determine MIME type */
  const char *mime = Mime::find(fi -> getName());
  if (mime) { response -> setHeader("Content-Type", mime); }
  /* Write file to response */
  int  fd = fi -> open(O_RDONLY, S_IREAD);
  int  count;
  char buffer[SZ_LINE_BUFFER];
  while ((count = read(fd, buffer, SZ_LINE_BUFFER)) > 0) {
    response -> write(buffer, count);
  }
}

void
Static::serve_dir(FileInfo *fi, HttpResponse *response) {
  /* Render Directory Entries */
  Buffer entries;

  FileInfo *child;
  while ((child = fi -> readdir())) {
    /* Skip . and .. */
    if (!child -> getName()) { continue; }
    if (!strcmp(child -> getName(), "..")) { continue; }

    render_file(child, &entries);
    delete child;
  }

  /* Render Response Page */
  Buffer result;
  render_dir(fi, &entries, &result);
  response -> write(result.data(), result.length());
}

void
Static::render_file(FileInfo *fi, Buffer *output) {
  Fragment entry(DEF_DIRITEM);
  entry.set("ent-name", fi -> getName());
  entry.set("ent-link", fi -> getName());
  entry.render(output);
}

void
Static::render_dir(FileInfo *fi, Buffer *entries, Buffer *output) {
  Fragment fragment(DEF_DIRLIST);
  fragment.set("dir-name", fi -> getName());
  fragment.set("dir-content", new Fragment(entries -> data(), entries -> length()));
  fragment.render(output);
}

void
Static::handle(HttpRequest *request, HttpResponse *response) {
  HttpServer *server = request -> server;
  Path path;
  path.push(root);
  path.push(request -> path);

  FileInfo fi(&path);

  switch (fi.getType()) {

    case FT_NOENT: {
      response -> setStatus(RES_404);
      server -> panic(request, response);
    } break;

    case FT_FILE: { serve_file(&fi, response); } break;

    case FT_DIR: {

      // If this is a directory and path does not have tailing slash,
      // we need to send back a 301.
      if (request -> path[strlen(request -> path) - 1] != '/') {
        response -> setStatus(RES_301);

        Buffer location;
        location.write(request -> path, strlen(request -> path));
        location.write("/\0", sizeof(char) * 2);
        response -> setHeader("Location", (char*)location.data());

        return;
      }


      /* Check if directory contains index.html */
      Path indexPath;
      indexPath.push(&path);
      indexPath.pushd("index.html");
      FileInfo index(&indexPath);
      if (index.exists()) {
        serve_file(&index, response);
        response -> setStatus(RES_200);
        return;
      }

      /* Handle opendir errors here */
      if (fi.opendir()) {
        switch (errno) {
          case EACCES: { response -> setStatus(RES_403); } break;
          case ENOENT: { response -> setStatus(RES_404); } break;
          default:     { response -> setStatus(RES_500); } break;
        }
        server -> panic(request, response);
        return;
      }

      /* Serve directory */
      serve_dir(&fi, response);

    } break;

  }

  response -> setStatus(RES_200);
}
