#include "include/transpose/fragment.h"
#include "include/transpose/buffer.h"
#include "include/corvo/headers.h"
#include "include/corvo/server.h"
#include "include/fs/fileinfo.h"
#include "include/fs/path.h"
#include "include/global.h"
#include "include/trace.h"
#include "src/serve.h"

#include "embed/direntry.html.h"
#include "embed/bootstrap.css.h"
#include "embed/listdir.html.h"
#include "embed/styles.css.h"
#include "embed/folder.svg.h"
#include "embed/file.svg.h"

#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void
serve(HttpRequest *request, HttpResponse *response) {
  HttpServer *server = request -> server;
  //Path *path = new Path(request -> path);
  Path *path = new Path(HTTP_ROOT);
  path -> push(request -> path);

  char *strPath = path -> str(); // TODO Remove this

  FileInfo *fi = new FileInfo(path);

  switch (fi -> getType()) {

    case FT_NOENT: {
      response -> setStatus(RES_404);
      server -> redirect("*", "!!error/404", request, response);
    } break;

    case FT_FILE: {
      int  fd = fi -> open(O_RDONLY, S_IREAD);
      int  count;
      char buffer[SZ_LINE_BUFFER];
      while ((count = read(fd, buffer, SZ_LINE_BUFFER)) > 0) {
        response -> write(buffer, count);
      }
    } break;

    case FT_DIR: {
      /* Handle opendir errors here */
      if (fi -> opendir()) {
        switch (errno) {
          case EACCES:
            response -> setStatus(RES_403);
            server -> redirect("*", "!!error/403", request, response);
            break;
          case ENOENT:
            response -> setStatus(RES_404);
            server -> redirect("*", "!!error/404", request, response);
            break;
          default:
            perror("FT_DIR");
            response -> setStatus(RES_500);
            server -> redirect("*", "!!error/500", request, response);
            break;
        }
        return;
      }

      /* Check if directory contains index.html */
      //Path *indexPath = new Path()

      /* Render Directory Entries */
      Buffer *buffer = new Buffer();
      FileInfo *child;
      while ((child = fi -> readdir())) {

        if (!child -> getName() || !strcmp(child -> getName(), "..")) { continue; }
        char *strEntryPath = child -> getPath() -> str();

        Fragment *entry = new Fragment(direntry_html);
        entry -> set("ent-icon", file_svg);
        entry -> set("ent-name", child -> getName());
        entry -> set("ent-link", strEntryPath);
        entry -> render(buffer);
        delete strEntryPath;
      }

      /* Render Response Page */
      Fragment *fragment = new Fragment(listdir_html);
      fragment -> set("css-bootstrap", bootstrap_css);
      fragment -> set("css-style", styles_css);
      fragment -> set("dir-name", path -> name());
      fragment -> set("dir-content", new Fragment(buffer -> data(), buffer -> length()));
      delete buffer;
      buffer = fragment -> render();

      response -> write(buffer -> data(), buffer -> length());
    } break;

  }

  response -> setStatus(RES_200);
  delete path;
}

void
serve_file(HttpResponse *response, const char *path) {

}

void
serve_dir(HttpResponse *response, const char *path) {

}
