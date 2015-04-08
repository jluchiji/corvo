#include "mime.h"

#define REG_OPTS REG_ICASE|REG_EXTENDED|REG_NOSUB

MimeList Mime::registry;

void
Mime::reg(const char *pattern, const char *mime) {
  Regex *regex = new Regex();
  if (regcomp(regex, pattern, REG_OPTS)) { delete regex; return; }

  MimePair *pair = new MimePair(regex, mime);
  registry.push_front(pair);
}

void
Mime::init() {
  reg(MIME_HTML);
  reg(MIME_CSS);
  reg(MIME_PLAIN);
  reg(MIME_XML);
  reg(MIME_GIF);
  reg(MIME_JPG);
  reg(MIME_PNG);
  reg(MIME_SVG);
  reg(MIME_TIFF);
  reg(MIME_JS);
  reg(MIME_JSON);
}

const char*
Mime::find(const char *path) {
  const char *mime = NULL;
  MimeList::iterator it = registry.begin();
  for (; it != registry.end(); ++it) {
    regmatch_t m;
    if (!regexec((*it) -> first, path, 1, &m, 0)) {
      mime = (*it) -> second;
      break;
    }
  }

  return mime;
}
