#include "include/transpose/fragment.h"
#include "include/transpose/buffer.h"
#include "include/corvo/headers.h"
#include "include/corvo/server.h"
#include "include/fs/path.h"
#include "include/global.h"
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
  Path *path = new Path(HTTP_ROOT);
  path -> push(request -> path);
  char *strPath = path -> str();

  /* Find the requested file */
  DirStat d_stat;
  if (stat(strPath, &d_stat)) {
    response -> setStatus(RES_404);
    HttpHandlerFunc errHandler =
      request -> server -> find_handler("*", "!!error/404");
    (*errHandler)(request, response);
    return;
  }

  /* If this is a file, serve it.. */
  if (!S_ISDIR(d_stat.st_mode)) { serve_file(response, strPath); }
  /* Directory.. list it */
  else {
    DIR      *dir = opendir(strPath);
    DirEntry *ent;

    /* Handle opendir errors here */
    if (!dir) {
      HttpHandlerFunc errHandler;
      switch (errno) {
        case EACCES:
          errHandler = request -> server -> find_handler("*", "!!error/403");
          break;
        case ENOENT:
          errHandler = request -> server -> find_handler("*", "!!error/404");
          break;
        default:
          errHandler = request -> server -> find_handler("*", "!!error/500");
          break;
      }

      (*errHandler)(request, response);
      return;
    }

    /* Render Directory Entries */
    Buffer *buffer = new Buffer();
    while ((ent = readdir(dir))) {

      if (!strcmp(ent -> d_name, ".") || !strcmp(ent -> d_name, "..")) { continue; }
      Path *entryPath = new Path(request -> path);
      entryPath -> pushd(ent -> d_name);
      char *strEntryPath = entryPath -> str();
      delete entryPath;

      Fragment *entry = new Fragment(direntry_html);
      entry -> set("ent-icon", file_svg);
      entry -> set("ent-name", ent -> d_name);
      entry -> set("ent-link", strEntryPath);
      entry -> render(buffer);
      delete strEntryPath;
      delete entry;
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

    return;
  }

  response -> setStatus(RES_200);
  delete path;
  delete strPath;
}

void
serve_file(HttpResponse *response, const char *path) {
  int  fd = open(strPath, O_RDONLY, S_IREAD);
  int  count;
  char buffer[SZ_LINE_BUFFER];
  while ((count = read(fd, buffer, SZ_LINE_BUFFER)) > 0) {
    response -> write(buffer, count);
  }
}

void
serve_dir(HttpResponse *response, const char *path) {

}
