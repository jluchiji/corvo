#ifndef _UTIL_H_
#define _UTIL_H_

#include "global.h"

#define RGX_SEGMENT "([^/]+)"

class IoUtil {
public:
  static ssize_t readline(int, char*, size_t);
};

#endif
