#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <string>

#include "corvo/util.h"
#include "fs/path.h"
#include "global.h"
#include "trace.h"

ssize_t
Util::readline(int fd, char *buffer, size_t n) {

  ssize_t   count   = 0;
  size_t    total   = 0;
  char     *buf     = buffer;
  char      current,
            last;

  while (1) {
    count = read(fd, &current, 1);

    /* Handle read failures and such */
    if (count < 0) { return -1; }
    else if (count == 0 && total == 0) { return 0; }
    else if (count == 0) { break; }

    /* Handle CRLF line endings: remove CR from line end */
    if (last == ASCII_CR && current == ASCII_LF) {
      *buf  -= 1; // Rewind by 1
      total -= 1; // Do not count CR into total
      break;
    }
    /* Handle LF line endings */
    if (current == ASCII_LF) {
      break;
    }

    /* Save last character for future loops */
    last = current;

    /* Do not read beyond the buffer size */
    if (total < (n - 1)) {
      total += 1;
      *buf++ = current;
    }
  }

  /* Null terminate and return */
  *buf = 0;
  return total;
}

Regex*
Util::toRegex(const char *path) {
  if (!path) return NULL;
  std::string *result = new std::string(path);

  Path::replace(result, ".", "\\.");
  Path::replace(result, "/", "\\/");
  Path::replace(result, "*", ".+");
  Path::replace(result, "?", ".");

  result -> insert(0, "^");
  result -> push_back('$');

  Regex *regex = new Regex();

  int      fail  = regcomp(regex, result -> c_str(), REG_EXTENDED|REG_NOSUB);
  if (fail) {
    COMPLAIN("%s: %s", result -> c_str(), strerror(errno));
    delete result;
    return NULL;
  }

  DBG_VERBOSE("Regex: %s\n", result -> c_str());
  delete result;
  return regex;
}
