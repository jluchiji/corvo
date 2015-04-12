// ------------------------------------------------------------------------- //
//                                                                           //
// CS252 Lab03 - Shell                                                       //
// Copyright © 2015 Denis Luchkin-Zhou                                       //
//                                                                           //
// puppet.cc                                                                 //
// This file contains the logic for the Puppet class that executes any       //
// program in a child process, while providing an interface for parent       //
// to access its exit status and read its output.                            //
//                                                                           //
// ------------------------------------------------------------------------- //
#include <string>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "trace.h"
#include "io/path.h"
#include "io/buffer.h"
#include "puppet.h"
#include "plumber.h"

// Forward declarations
Path *Puppet::_root;
Path *Puppet::_self;

// ------------------------------------------------------------------------- //
// Constructor. Creates a puppet instance from an executable path.           //
// ------------------------------------------------------------------------- //
Puppet::Puppet(const char* exe) {
  if (!Puppet::_root) {
    PANIC("You must call Puppet::init() first!\n");
  }

  finalized     = false;
  _status.exit  = 0;
  _status.error = 0;

  // Compute executable path
  Path *path = new Path(exe);
  if (path -> isAbsolute()) {
    executable = path -> str();
  }
  else {
    executable = path -> resolve(Puppet::_root);
  }

  delete path;

  // Create pipes
  if (pipe(ipipe) == -1 ||
      pipe(opipe) == -1 ||
      pipe(epipe) == -1) {
    COMPLAIN("Puppet: pipe: %s\n", strerror(errno));
    finalized = true;
  }
}

// ------------------------------------------------------------------------- //
// Destructor. Closes read ends of stdout and stderr pipes.                  //
// ------------------------------------------------------------------------- //
Puppet::~Puppet() {
  free(executable);
  close(opipe[0]);
  close(epipe[0]);
}

// ------------------------------------------------------------------------- //
// Writes a string to the stdin of the puppet process.                       //
// Returns pointer to itself for easy chaining.                              //
// ------------------------------------------------------------------------- //
Puppet*
Puppet::write(const char *str) {
  if (finalized) {
    COMPLAIN("puppet: write: Cannot write to a finalized puppet.\n");
    return this;
  }
  dprintf(ipipe[1], "%s", str);
  return this;
}

// ------------------------------------------------------------------------- //
// Reads entire content of the puppet's stdout or stderr depending on the    //
// value of `type`. Possible values are IO_OUT or IO_ERR. Does not close     //
// pipe after reading, but lseeks to the beginning. Blocks until the puppet  //
// closes the pipe that is being read from.                                  //
// ------------------------------------------------------------------------- //
Buffer*
Puppet::read(int type) {

  // Figure out which stream to read
  int fd;
  switch (type) {
    case IO_OUT: fd = opipe[0]; break;
    case IO_ERR: fd = epipe[0]; break;
    default:
      COMPLAIN("puppet: read: bad stream type [%d]", type);
      return NULL;
  }

  // Temporary buffer and std::string for dynamic growth
  char buffer[PUPPET_BUFFER_SIZE];
  Buffer *data = new Buffer(PUPPET_BUFFER_SIZE);

  // Read the entire buffer
  int count;
  while ((count = ::read(fd, buffer, PUPPET_BUFFER_SIZE))) {
    data -> write(buffer, count);
  }

  // Rewind so that it can be read again if needed
  // Output ends of output pipes are closed by destructor
  lseek(fd, 0, SEEK_SET);

  return data;
}

// ------------------------------------------------------------------------- //
// Reads contents of the puppet's stdout or stderr depending on the value    //
// of the `type`. Possible values are IO_OUT or IO_ERR. Does not close pipe  //
// after reading and does nothing to reset the position. May block until     //
// the puppet closes the writing end of the pipe.                            //
// ------------------------------------------------------------------------- //
size_t
Puppet::read(int type, char *buffer, size_t count) {

  // Figure out which stream to read
  int fd;
  switch (type) {
    case IO_OUT: fd = opipe[0]; break;
    case IO_ERR: fd = epipe[0]; break;
    default:
      COMPLAIN("puppet: read: bad stream type [%d]", type);
      return 0;
  }

  // Read and return
  return ::read(fd, buffer, count);
}

// ------------------------------------------------------------------------- //
// Executes the puppet program. Takes care of piping and forking.            //
// Returns pointer to itself for easy chaining.                              //
// ------------------------------------------------------------------------- //
Puppet*
Puppet::run() {
  if (finalized) {
    COMPLAIN("puppet: finalized: Cannot run a finalized puppet.");
    _status.exit = -1;
    return this;
  }

  close(ipipe[1]);

  // Prevent repeated execution
  finalized = true;

  // Set up plumber
  Plumber *plumber = new Plumber();
  plumber -> file(IO_IN,  ipipe[0]);
  plumber -> file(IO_OUT, opipe[1]);
  plumber -> file(IO_ERR, epipe[1]);

  // Check whether executable exists
  if (access(executable, X_OK)) {
    _status.exit = -1;
    _status.error = errno;
  }

  // Fork if everything looks good
  int pid = -1;
  if (!_status.exit && !_status.error) {
    pid = fork();
    if (pid == -1) {
      COMPLAIN("puppet: fork: %s", strerror(errno));
      _status.exit = -1;
      return this;
    }

    // Redirect IO
    plumber -> redirect(IO_IN);
    plumber -> redirect(IO_OUT);
    plumber -> redirect(IO_ERR);

    // Execute
    if (pid == 0) {
      execlp(executable, executable, NULL);
      exit(PUPPET_EXIT_BASE + errno);
    }
  }

  // Restore IO
  delete plumber;
  close(ipipe[0]);
  close(opipe[1]);
  close(epipe[1]);

  // Wait for process to exit and return its exit _status
  if (pid != -1) {
    int exit_status;
    waitpid(pid, &exit_status, 0);
    if (WIFEXITED(exit_status)) {
      _status.exit = WEXITSTATUS(exit_status);
    }
    else {
      _status.exit = 1;
    }
    if (_status.exit > PUPPET_EXIT_BASE) {
      _status.error = _status.exit - PUPPET_EXIT_BASE;
      _status.exit  = 1;
    }
  }

  return this;
}

// ------------------------------------------------------------------------- //
// Returns the exit status of the puppet process.                            //
// ------------------------------------------------------------------------- //
PuppetStatus*
Puppet::status() {
  return &_status;
}

// ------------------------------------------------------------------------- //
// Initializes the path resolution mechanism of the Puppet.                  //
// Essentially, it will set cwd() as the root path for the resolution.       //
// ------------------------------------------------------------------------- //
void
Puppet::init(char *path) {
  Puppet::_self = new Path(path);
  if (Puppet::_self -> isAbsolute()) {
    // CWD cannot be used as root here
    Puppet::_root = Puppet::_self -> clone() -> pushd("..");
  }
  else {
    // Relative path, so CWD can be used as root
    Puppet::_root = Path::cwd() -> push(Puppet::_self) -> pushd("..");
  }
  while (Puppet::_self -> count() > 1) { free(Puppet::_self -> popd()); }
}

// ------------------------------------------------------------------------- //
// Creates an puppet instance of the parent process.                         //
// ------------------------------------------------------------------------- //
Puppet*
Puppet::self() {
  char *path = Puppet::_self -> str();
  Puppet *p = new Puppet(path);
  free(path);
  return p;
}
