// ------------------------------------------------------------------------- //
//                                                                           //
// CS252 Lab03 - Shell                                                       //
// Copyright © 2015 Denis Luchkin-Zhou                                       //
//                                                                           //
// plumber.h                                                                 //
// This file contains definitions for Plumber, the pipe master of the shell. //
//                                                                           //
// ------------------------------------------------------------------------- //
#ifndef PLUMBER_H_
#define PLUMBER_H_

// ------------------------------------------------------------------------- //
// Shorthands for open() flags.                                              //
// ------------------------------------------------------------------------- //
#define F_READ   O_RDONLY
#define F_TRUNC  O_WRONLY | O_CREAT | O_TRUNC
#define F_APPEND O_WRONLY | O_CREAT | O_APPEND

// ------------------------------------------------------------------------- //
// Plumber redirect constants.                                               //
//   PLB_NONE : No redirect; or redirect to file if one is specified.        //
//   PLB_PIPE : Redirect to an appropriate pipe held by Plumber.             //
// ------------------------------------------------------------------------- //
#define PLB_NONE  -1
#define PLB_PIPE  -2

// ------------------------------------------------------------------------- //
// Plumber std IO constants. Self explanatory.                               //
// ------------------------------------------------------------------------- //
#define IO_IN      0
#define IO_OUT     1
#define IO_ERR     2

// ------------------------------------------------------------------------- //
// Plumber the pipe master.                                                  //
// ------------------------------------------------------------------------- //
class Plumber {
private:
  int       _std[3];                   // Final output handles. DO NOT DUP!
  int       _def[3];                   // Default I/O handles

public:
  Plumber();                           // Constructor. Captures current IO
  ~Plumber();                          // Destructor. Restores captured IO

  bool push();                         // Push a new pipe into the Plumber
  void file(int, int);                 // Queues a descriptor for redirect
  bool file(int, char*, bool);         // Queues a file for redirect
  void redirect(int);                  // Applies redirect to an std IO
};

#endif
