#include <unistd.h>
#include <stdio.h>
#include "global.h"
#include "util.h"

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
