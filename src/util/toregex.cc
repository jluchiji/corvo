#include "toregex.h"
#include "io/path.h"
#include "trace.h"

#include <string.h>
#include <errno.h>
#include <string>

Regex* toRegex(const char* pattern) {
  if (!pattern) return NULL;
  std::string *result = new std::string(pattern);

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
