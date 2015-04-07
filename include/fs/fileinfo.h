#ifndef _FILEINFO_H_
#define _FILEINFO_H_

#include <dirent.h>
#include "fs/path.h"

/* Type of file entry */
enum FileType {
  FT_NOENT,
  FT_FILE,
  FT_DIR
};

/* FileInfo class */
class FileInfo {
private:
  DIR           *dir;

  Path          *path;
  FileType       type;
  unsigned long  size;

  void        init(Path*);

public:

  FileInfo(Path*);
  FileInfo(const char*);

  bool        exists();
  const char* getName();
  FileType    getType();
  Path*       getPath();

  int         open(int, int);
  bool        opendir();
  FileInfo*   readdir();
};

#endif
