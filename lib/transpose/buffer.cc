// ------------------------------------------------------------------------- //
//                                                                           //
// CS252 Lab05 - HTTP Server                                                 //
// Copyright © 2015 Denis Luchkin-Zhou                                       //
//                                                                           //
// buffer.cc                                                                 //
// This file contains implementation for the buffer class that powers our    //
// dynamic templating and response buffering.                                //
//                                                                           //
// ------------------------------------------------------------------------- //
#include <cstdlib>
#include <cstring>

#include "include/transpose/buffer.h"

// ------------------------------------------------------------------------- //
// Constructor. Creates a Buffer instance with the default capacity.         //
// ------------------------------------------------------------------------- //
Buffer::Buffer() {
  this -> capacity = SZ_INIT_BUFFER;
  content = new char[capacity];
  size = 0;
}

// ------------------------------------------------------------------------- //
// Constructor. Creates a Buffer instance with the specified capacity.       //
// ------------------------------------------------------------------------- //
Buffer::Buffer(size_t capacity) {
  this -> capacity = capacity;
  content = new char[capacity];
  size = 0;
}

// ------------------------------------------------------------------------- //
// Destructor. Deletes the data stored in the Buffer instance.               //
// ------------------------------------------------------------------------- //
Buffer::~Buffer() { delete content; }

// ------------------------------------------------------------------------- //
// Writes arbitrary data into the buffer, expanding capacity as needed.      //
// ------------------------------------------------------------------------- //
void
Buffer::write(const void *buffer, size_t n) {
  const char *buf = (const char*)buffer;
  /* Ensure that there is sufficient capacity */
  if (capacity - size < n) {
    while (capacity - size < n) { capacity *= 2; }
    content = (char*) realloc(content, capacity);
  }
  /* Copy data */
  memcpy(content + size, buf, n);
  size += n;
}

// ------------------------------------------------------------------------- //
// Gets the number of bytes stored in the Buffer instance.                   //
// ------------------------------------------------------------------------- //
size_t
Buffer::length() { return size; }

// ------------------------------------------------------------------------- //
// Gets the pointer to the data stored in the Buffer.                        //
// Note that data pointed by the pointer is still a part of the Buffer       //
// instance, therefore deleting the Buffer instance deletes it as well!      //
// ------------------------------------------------------------------------- //
void*
Buffer::data() { return content; }
