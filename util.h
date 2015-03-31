#ifndef _UTIL_H_
#define _UTIL_H_

#define RGX_SEGMENT "([^/]+)"

class Util {
public:

  static ssize_t readline(int, char*, size_t);
  static Regex*  toRegex(const char*);
};

#endif
