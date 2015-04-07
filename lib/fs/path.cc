// ------------------------------------------------------------------------- //
//                                                                           //
// CS252 Lab03 - Shell                                                       //
// Copyright © 2015 Denis Luchkin-Zhou                                       //
//                                                                           //
// path.cc                                                                   //
// This file contains the logic for the class handling file path             //
// manipulation. This class forms the cornerstone of globbing.               //
//                                                                           //
// ------------------------------------------------------------------------- //

/*
 * Three Rings for the Elven-kings under the sky,
 * Seven for the Dwarf-lords in halls of stone,
 * Nine for Mortal Men, doomed to die,
 * One for the Dark Lord on his dark throne
 * In the Land of Mordor where the Shadows lie.
 * One Ring to rule them all, One Ring to find them,
 * One Ring to bring them all and in the darkness bind them.
 * In the Land of Mordor where the Shadows lie.
 *   -- J.R.R. Tolkien
 */

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <string>
#include "trace.h"
#include "fs/path.h"

// ------------------------------------------------------------------------- //
// Default constructor. Creates a relative Path object with no segments.     //
// ------------------------------------------------------------------------- //
Path::Path() { init(NULL); }

// ------------------------------------------------------------------------- //
// Constructor. Creates a path from its string representation.               //
// This constructor copies contents of the string, therefore the caller is   //
// responsible for freeing the memory.                                       //
// ------------------------------------------------------------------------- //
Path::Path(const char* str) { init(str); }

// ------------------------------------------------------------------------- //
// Destructor. Deletes all strings managed by this path, including segments. //
// ------------------------------------------------------------------------- //
Path::~Path() {
  // Delete all segments
  while (!segments -> empty()) {
    char *segment = segments -> front();
    segments -> pop_front();
    free(segment);
  }
}

// ------------------------------------------------------------------------- //
// Initializes the path instance from a string representation.               //
// If str is NULL, the instance will be an empty relative path.              //
// ------------------------------------------------------------------------- //
void
Path::init(const char* str) {
  const char *ps = str,
             *pe = str;
  // Initialize class members
  absolute = 0;
  segments = new SegmentList();
  // Watch out for null argument!
  if (!str) { return; }
  // Paths starting with '/' are absolute
  if (*ps == '/') { absolute = 1; ps += 1; pe = ps; }
  // Split paths into segments
  while (*pe++) {
    if (*pe == '/' && pe - ps > 0) {
      segments -> push_back(strndup(ps, pe - ps));
      ps = pe + 1;
    }
  }
  // Copy over the final segment
  if (pe - ps > 0) { segments -> push_back(strdup(ps)); }
}

// ------------------------------------------------------------------------- //
// Pushes a string path into the path object.                                //
// Resolves '.' and '..' segments where possible.                            //
// Copies the contents of the `path`, therefore the caller is responsible    //
// for freeing the memory.                                                   //
// ------------------------------------------------------------------------- //
Path*
Path::push(const char *path) {
  Path *p = new Path(path);
  push(p);
  delete p;
  return this;
}

// ------------------------------------------------------------------------- //
// Pushes segments of another path into the path object.                     //
// Does not alter the `path` pbject.                                         //
// Copies segments of the `path`, therefore the caller is responsible        //
// for freeing the memory.                                                   //
// ------------------------------------------------------------------------- //
Path*
Path::push(Path *path) {
  SegmentList::iterator it = path -> segments -> begin();
  for (; it != path -> segments -> end(); ++it) { pushd(*it); }
  return this;
}

// ------------------------------------------------------------------------- //
// Pushes one segment into the path.                                         //
// Technically, Path::push() is more flexible than this, but when you need   //
// to insert segments without resolving '.' and '..', try this one.          //
// Copies the `segment`, therefore the caller is responsible for freeing the //
// memory.                                                                   //
// ------------------------------------------------------------------------- //
Path*
Path::pushd(const char* segment) {
  if (!segment) { return this; }

  // Get rid of heading and tailing slashes
  char *seg  = Path::trim(segment, '/');

  // If this is a '.', no need to push it at all!
  if (!strcmp(seg, ".")) { /* skip this one */ }

  // If this is a '..', pop a directory from back
  else if (!strcmp(seg, "..") &&                 // We must have the '..', and
           !segments -> empty() &&               // a segment to pop, and
            strcmp(segments -> back(), "..")) {  // that segment can't be '..'
    char *last = segments -> back();
    segments -> pop_back();
    free(last);
    free(seg);
  }

  // So we either really need to push the segment, or we have
  // nothing left to pop.
  else {
    segments -> push_back(seg);
  }

  return this;
}

// ------------------------------------------------------------------------- //
// Pops the first segment og the path.                                       //
// Data returned by this method is no longer managed by the Path object,     //
// therefore it is up to the caller to free the return value.                //
// ------------------------------------------------------------------------- //
char*
Path::popd() {
  if (segments -> empty()) { return NULL; }
  char *segment = segments -> front();
  segments -> pop_front();
  return segment;
}

// ------------------------------------------------------------------------- //
// Resolves unresolved '.' and '..' segments where possible.                 //
// ------------------------------------------------------------------------- //
Path*
Path::reduce() {
  SegmentList *backup   = segments;
               segments = new SegmentList();
  while (!backup -> empty()) {
    char *segment = backup -> front();
    backup -> pop_front();
    pushd(segment);
    free(segment);
  }
  delete backup;
  return this;
}

// ------------------------------------------------------------------------- //
// DEBUG ONLY: Prints the string representation of the path.                 //
// ------------------------------------------------------------------------- //
Path*
Path::print() {
  char *p = str();
  DBG_INFO("Path::print(): %s\n", p);
  free(p);
  return this;
}

// ------------------------------------------------------------------------- //
// Gets the name (i.e. the last segment) of the path object.                 //
// The string returned is still a part of the path, so do not delete it!     //
// ------------------------------------------------------------------------- //
const char*
Path::name() {
  if (segments -> empty()) { return NULL; }
  const char *result = NULL;
  SegmentList::iterator it = segments -> begin();
  for (; it != segments -> end(); ++it) {
    if (strlen(*it)) { result = *it; }
  }
  return result;
}


// ------------------------------------------------------------------------- //
// Creates a string representation of the Path object.                       //
// Data returned by this method is not managed by the Path object, therefore //
// it is up to the caller to free the return value.                          //
// ------------------------------------------------------------------------- //
char*
Path::str() {
  // If this is an empty relative path, just return the '.'
  if (!absolute && segments -> empty()) {
    return strdup(".");
  }

  char  buffer[PATH_MAX];
  char *pw = buffer;

  // Reset the buffer
  memset(buffer, sizeof(char) * PATH_MAX, 0);

  // New queue to insert items
  SegmentList *nseg     = segments;
               segments = new SegmentList();

  // Start absolute path with a slash
  if (absolute) { *pw++ = '/'; }

  while (!nseg -> empty()) {
    char *pr      = nseg -> front(),
         *segment = pr;

    nseg -> pop_front();

    // Skip empty segments
    if (!strlen(segment)) { continue; }

    while (*pr) { *pw++ = *pr++; }
    if (!nseg -> empty()) { *pw++ = '/'; }
    segments -> push_back(segment);
  }
  *pw = 0;
  assert(nseg -> empty());
  delete nseg;


  return strdup(buffer);
}

// ------------------------------------------------------------------------- //
// Resolves a path relative to another path.                                 //
// Does not modify either of paths.                                          //
// Data returned by this method is not managed by the Path object, therefore //
// it is up to the caller to free the return value.                          //
// ------------------------------------------------------------------------- //
char*
Path::resolve(Path *base) {
  Path *resolved = base -> clone() -> push(this);
  char *strpath = resolved -> str();
  delete resolved;
  return strpath;
}

// ------------------------------------------------------------------------- //
// Returns 1 if the path is an absolute path; 0 otherwise.                   //
// ------------------------------------------------------------------------- //
int
Path::isAbsolute() {
  return absolute;
}

// ------------------------------------------------------------------------- //
// If called with 0 as arguments, makes the path relative.                   //
// Otherwise, makes the path absolute.                                       //
// ------------------------------------------------------------------------- //
void
Path::setAbsolute(int value) {
  absolute = value;
}

// ------------------------------------------------------------------------- //
// Returns the number of segments in the path.                               //
// ------------------------------------------------------------------------- //
int
Path::count() {
  return segments -> size();
}

// ------------------------------------------------------------------------- //
// Creates a deep copy of the path object.                                   //
// It is the caller's responsibility to free the return value.               //
// ------------------------------------------------------------------------- //
Path*
Path::clone() {
  Path *np = new Path();
  np -> absolute = absolute;
  SegmentList::iterator it = segments -> begin();
  for (; it != segments -> end(); ++it) {
    np -> segments -> push_back(strdup(*it));
  }
  return np;
}


// ------------------------------------------------------------------------- //
// Returns a value that indicates whether the given path exists in the file  //
// system.                                                                   //
// ------------------------------------------------------------------------- //
bool
Path::exists(const char *path) {
  struct stat d_stat;
  if (stat(path, &d_stat)) { return true; }
  else { return false; }
}

// ------------------------------------------------------------------------- //
// Removes backslashes from escape sequences.                                //
// Does not support interpreting excape sequences such as \n or \t.          //
// Directly modifies the string passed in as a parameter.                    //
// ------------------------------------------------------------------------- //
void
Path::unescape(char* str) {
  char *pw = str, *pr = str;
  while (*pr) {
    if (*pr == '\\') { pr++; }
    *pw++ = *pr++;
  }
  *pw = 0;
}

// ------------------------------------------------------------------------- //
// Replaces all occurences of `find` in `str` with `replace`.                //
// ------------------------------------------------------------------------- //
void
Path::replace(std::string *str, const char *find, const char *replace) {
  int pos  = 0,
      flen = strlen(find),
      rlen = strlen(replace);
  while ((pos = str->find(find, pos)) != std::string::npos) {
    str -> replace(pos, flen, replace);
    pos += rlen;
  }
}

// ------------------------------------------------------------------------- //
// Removes all occurences of the `c` from the end of `str`.                  //
// Directly modifies the string passed in as a parameter.                    //
// ------------------------------------------------------------------------- //
void
Path::trimend(char* str, char c) {
  char *pw = str + strlen(str) - 1;
  while (*pw == c) { *pw-- = 0; }
}

// ------------------------------------------------------------------------- //
// Removes all occurences of the specified character from start and end      //
// of string.                                                                //
// Creates a copy of the string passed in as the parameter, therefore the    //
// caller is responsible for freeing the return value.                       //
// ------------------------------------------------------------------------- //
char*
Path::trim(const char* str, char c) {
  const char *ps = str,
             *pe = str + strlen(str) - 1;
  while (*ps == c) { ps++; }
  while (*pe == c) { pe--; }
  return strndup(ps, pe - ps + 1);
}


Path*
Path::cwd() {
  char tmp[2048];
  memset(tmp, sizeof(char) * 2048, 0);
  if (getcwd(tmp, sizeof(tmp))) {
    return new Path(tmp);
  }
  return NULL;
}
