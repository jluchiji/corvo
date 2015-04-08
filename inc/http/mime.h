#ifndef _MIME_H_
#define _MIME_H_

#include "global.h"
#include <list>

// ------------------------------------------------------------------------- //
// Common MIME types for the server.                                         //
// ------------------------------------------------------------------------- //
#define MIME_HTML     ".html$",        "text/html"
#define MIME_CSS      ".css$",         "text/css"
#define MIME_PLAIN    ".txt$",         "text/plain"
#define MIME_XML      ".xml$",         "text/xml"
#define MIME_GIF      ".gif$",         "image/gif"
#define MIME_JPG      ".(jpg|jpeg)$",  "image/jpeg"
#define MIME_PNG      ".png$",         "image/png"
#define MIME_SVG      ".svg$",         "image/svg+xml"
#define MIME_TIFF     ".tiff$",        "image/tiff"
#define MIME_JS       ".(js|es6)$",    "application/javascript"
#define MIME_JSON     ".json$",        "application/json"

// ------------------------------------------------------------------------- //
// typedefs for convenience.                                                 //
// ------------------------------------------------------------------------- //
typedef std::pair<Regex*,const char*> MimePair;
typedef std::list<MimePair*> MimeList;

// ------------------------------------------------------------------------- //
// Static MIME type map.                                                     //
// ------------------------------------------------------------------------- //
class Mime {
private:

  static MimeList    registry;

public:

  static void         reg(const char*, const char*);
  static void         init();
  static const char*  find(const char*);

};

#endif
