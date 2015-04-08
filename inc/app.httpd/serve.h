#ifndef _SERVE_H_
#define _SERVE_H_

#include "io/path.h"
#include "io/fileinfo.h"
#include "http/request.h"
#include "http/response.h"
#include "middleware/static.h"

#define HTTP_ROOT "http-root-dir"

class StaticFileServer : public StaticFile {
private:

  void render_file(FileInfo*, Buffer*);
  void render_dir(FileInfo*, Buffer*, Buffer*);

public:

  StaticFileServer(const char*);

};

#endif
