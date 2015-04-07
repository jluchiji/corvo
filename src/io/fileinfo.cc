#include "fileinfo.h"

#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

FileInfo::FileInfo(Path *path) {
  this -> init(path -> clone());
}

FileInfo::FileInfo(const char *path) {
  this -> init(new Path(path));
}

void
FileInfo::init(Path *path) {
  this -> path = path;
  char *strPath = this -> path -> str();

  /* Get file stats */
  struct stat d_stat;
  int fail = stat(strPath, &d_stat);
  delete strPath;

  /* Check if file exists */
  if (fail) {
    type = FT_NOENT;
    return;
  }

  /* Is this a dir or a file? */
  if (S_ISDIR(d_stat.st_mode)) {
    type = FT_DIR;
  }
  else {
    type = FT_FILE;
  }

  /* Get file size */
  size = d_stat.st_size;
}


bool
FileInfo::exists() {
  return this -> type != FT_NOENT;
}

const char*
FileInfo::getName() {
  return path -> name();
}

FileType
FileInfo::getType() {
  return type;
}

Path*
FileInfo::getPath() {
  return path;
}

int
FileInfo::open(int access, int permission) {
  if (type == FT_NOENT) {
    errno = ENOENT;
    return -1;
  }
  if (type == FT_DIR) {
    errno = EISDIR;
    return -1;
  }
  char *strPath = path -> str();
  int fd = ::open(strPath, access, permission);
  delete strPath;
  return fd;
}

bool
FileInfo::opendir() {
  if (type == FT_NOENT) {
    errno = ENOENT;
    return true;
  }
  if (type == FT_FILE) {
    errno = ENOTDIR;
    return true;
  }
  char *strPath = path -> str();
  dir = ::opendir(strPath);
  delete strPath;
  return !dir;
}

FileInfo*
FileInfo::readdir() {
  struct dirent *entry = ::readdir(dir);
  while (entry && (!strcmp(entry -> d_name, ".") ||
         !strcmp(entry -> d_name, ".."))) {
    entry = ::readdir(dir);
  }
  if (!entry) return NULL;

  Path *child = path -> clone() -> pushd(entry -> d_name);
  return new FileInfo(child);
}
