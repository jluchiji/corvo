// ------------------------------------------------------------------------- //
//                                                                           //
// CS252 Lab03 - Shell                                                       //
// Copyright © 2015 Denis Luchkin-Zhou                                       //
//                                                                           //
// puppet.h                                                                  //
// This file contains definitions for the Puppet class that executes any     //
// program in a child process, while providing an interface for parent       //
// to access its exit status and read its output.                            //
//                                                                           //
// ------------------------------------------------------------------------- //

#ifndef PUPPET_H_
#define PUPPET_H_

#include "plumber.h"

// ------------------------------------------------------------------------- //
// Buffer size for reading puppet output.                                    //
// ------------------------------------------------------------------------- //
#define PUPPET_BUFFER_SIZE 1024

// ------------------------------------------------------------------------- //
// Puppet's child process exit code when there is an error.                  //
// ------------------------------------------------------------------------- //
#define PUPPET_EXIT_BASE 128

// ------------------------------------------------------------------------- //
// Struct representing exit state of the puppet process.                     //
// ------------------------------------------------------------------------- //
typedef struct {
  int   exit;                        // Exit code emitted by the child process
  int   error;                       // Error number that caused the error
} PuppetStatus;

// ------------------------------------------------------------------------- //
// Puppet class, runs a program with all kinds of strings attached.          //
// ------------------------------------------------------------------------- //
class Puppet {
private:
  char     *executable;              // Absolute path of the puppet executable

  int          ipipe[2];             // Puppet's stdin
  int          opipe[2];             // Puppet's stdout
  int          epipe[2];             // Puppet's stderr
  PuppetStatus _status;              // Exit status of the puppet process

  bool      finalized;               // Flag that indicates whether the puppet
                                     // input is writable
  static Path *_root;                // Root path for Puppet path resolutions
  static Path *_self;                // Path of the bush executable
public:
  Puppet(const char*);               // Constructor, creates a puppet instance
                                     // from an executable path
  ~Puppet();                         // Destructor, cleans up the mess

  Puppet*        write(const char*); // Writes data to the puppet's stdin
  char*          read(int);          // Reads entire content of puppet's stdout
                                     // or stderr, depending on its parameter
  Puppet*        run();              // Executes the puppet program. Note that
                                     // after calling this function, all writes
                                     // to the puppet's stdin are disabled.
  PuppetStatus*  status();           // Returns the exit status of puppet process

  static void    init(char*);        // Initializes the Puppet's path resolution
                                     // Pass in the argv[0] from the main()
  static Puppet* self();             // Creates a puppet of the parent process
};

#endif
