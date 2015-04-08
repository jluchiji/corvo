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

Serve::Serve(const char* root) : Static(root) {

}

void
Serve::render_file(FileInfo *fi, Buffer *output) {
  Fragment entry(RES_DIRENTRY);
  entry.set("ent-name", fi -> getName());

  /* Set icon & link depending on type */
  if (fi -> getType() == FT_DIR) {
    /* Generate proper link */
    Buffer link;
    link.write(fi -> getName(), strlen(fi -> getName()));
    link.write("/\0", sizeof(char) * 2);

    entry.set("ent-icon", RES_FOLDER);
    entry.set("ent-link", (char*)link.data());
  }
  else {
    entry.set("ent-icon", RES_FILE);
    entry.set("ent-link", fi -> getName());
  }

  entry.render(output);
}

void
Serve::render_dir(FileInfo *fi, Buffer *entries, Buffer *output) {
  Fragment page(RES_LISTDIR);
  page.set("css-bootstrap", RES_BOOTSTRAP);
  page.set("css-style", RES_STYLES);
  page.set("dir-name", fi -> getName());
  page.set("dir-content", new Fragment(entries -> data(), entries -> length()));
  page.render(output);
}
