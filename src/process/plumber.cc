// ------------------------------------------------------------------------- //
//                                                                           //
// CS252 Lab03 - Shell                                                       //
// Copyright © 2015 Denis Luchkin-Zhou                                       //
//                                                                           //
// plumber.cc                                                                //
// This file contains logic for Plumber, the pipe master of the shell.       //
//                                                                           //
// ------------------------------------------------------------------------- //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <sys/stat.h>

#include "trace.h"
#include "plumber.h"


// ------------------------------------------------------------------------- //
// Constructor. Captures current standard IO handles.                        //
// ------------------------------------------------------------------------- //
Plumber::Plumber() {
  // Capture current stdin, stdout and stderr
  _def[0]  = dup(0);
  _def[1]  = dup(1);
  _def[2]  = dup(2);
}

// ------------------------------------------------------------------------- //
// Destructor. Restores the captured standard IO handles.                    //
// ------------------------------------------------------------------------- //
Plumber::~Plumber() {
  // Restore default IO
  dup2(_def[0], 0);
  dup2(_def[1], 1);
  dup2(_def[2], 2);

  // Close backup IO descriptors
  close(_def[0]);
  close(_def[1]);
  close(_def[2]);
}

// ------------------------------------------------------------------------- //
// Queues a file path for redirect on the specific standard stream.          //
// ------------------------------------------------------------------------- //
bool
Plumber::file(int type, char *path, bool append) {

  // Compute open flags for the file
  int flags;
  switch (type) {
    case IO_IN:  { flags = F_READ; } break;
    case IO_ERR:
    case IO_OUT: { flags = append ? F_APPEND : F_TRUNC; } break;
    default: { PANIC("Plumber::_iofile(): Unexpected iofile type: %d\n", type); }
  }

  // If the path is not null, open the file and redirect stdin from it
  if (path) {
    DBG_INFO("Plubmer::_iofile(): [%d] %s\n", type, path);

    _std[type] = open(path, flags, S_IRUSR | S_IWUSR | S_IXUSR);

    // Fail miserably if the file refuses to open
    if (_std[type] == -1) {
      COMPLAIN("%s: %s\n", path, strerror(errno));
      return false;
    }

  }
  // Otherwise, just read from stdin as always
  else {
    DBG_INFO("Plubmer::_iofile(): [%d] default\n", type);
    _std[type] = dup(_def[type]);
  }
  return true;
}

// ------------------------------------------------------------------------- //
// Queues a file descriptor for redirect on the specific standard stream.    //
// ------------------------------------------------------------------------- //
void
Plumber::file(int type, int fd) {
  _std[type] = fd;
}

// ------------------------------------------------------------------------- //
// Applies a queued redirect to the specified std IO.                        //
// ------------------------------------------------------------------------- //
void
Plumber::redirect(int i) {
  DBG_INFO("Plubmer::apply(): %d\n", i);
  dup2(_std[i], i);
  close(_std[i]);
}

// ------------------------------------------------------------------------- //
// Creates a new pipe and pushes it as the new Plumber state.                //
// ------------------------------------------------------------------------- //
bool
Plumber::push() {
  int fdpipe[2];
  if (pipe(fdpipe) == -1) {
    COMPLAIN("%s: %s\n", "pipe", strerror(errno));
    return false;
  }
  _std[1] = fdpipe[1];
  _std[0] = fdpipe[0];
  return true;
}
