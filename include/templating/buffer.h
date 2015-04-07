// ------------------------------------------------------------------------- //
//                                                                           //
// CS252 Lab05 - HTTP Server                                                 //
// Copyright © 2015 Денис Лучкин-Чжоу                                        //
//                                                                           //
// buffer.h                                                                  //
// Данный файл содержит обжявление класса Buffer, который позволяет нам      //
// динамично вставлять текст в шаблоны.                                      //
//                                                                           //
// ------------------------------------------------------------------------- //
#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <cstdlib>

// ------------------------------------------------------------------------- //
// Initial buffer size in bytes.                                             //
// ------------------------------------------------------------------------- //
#define SZ_INIT_BUFFER 1024

// ------------------------------------------------------------------------- //
// Buffer class: write like there is no tomorrow and never overflow again.   //
// ------------------------------------------------------------------------- //
class Buffer {
private:
  char    *content;                    // Data stored in the buffer
  size_t   size;                       // Number of bytes stored in buffer
  size_t   capacity;                   // Capacity of the content array

public:
  Buffer();                            // Creates a buffer of default size
  Buffer(size_t);                      // Creates a buffer of specified size
  ~Buffer();                           // Deletes buffer and data stored

  void   write(const void*, size_t);   // Writes data to the buffer
  size_t length();                     // Gets the number of bytes stored
  void*  data();                       // Gets the actual data stored
};

#endif
