// ------------------------------------------------------------------------- //
//                                                                           //
// CS252 Lab05 - HTTP Server                                                 //
// Copyright © 2015 Denis Luchkin-Zhou                                       //
//                                                                           //
// trace.h                                                                   //
// This file contains macros for debugging and console output coloring.      //
//                                                                           //
// ------------------------------------------------------------------------- //

#ifndef TRACE_H
#define TRACE_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#ifndef NO_COLORS

  #define COLOR_BLACK   "\033[0;30m"
  #define COLOR_BLUE    "\033[0;34m"
  #define COLOR_GREEN   "\033[0;32m"
  #define COLOR_CYAN    "\033[0;36m"
  #define COLOR_RED     "\033[0;31m"
  #define COLOR_PURPLE  "\033[0;35m"
  #define COLOR_BROWN   "\033[0;33m"
  #define COLOR_LGRAY   "\033[0;37m"
  #define COLOR_DGRAY   "\033[1;30m"
  #define COLOR_LBLUE   "\033[1;34m"
  #define COLOR_LGREEN  "\033[1;32m"
  #define COLOR_LCYAN   "\033[1;36m"
  #define COLOR_LRED    "\033[1;31m"
  #define COLOR_LPURPLE "\033[1;35m"
  #define COLOR_YELLOW  "\033[1;33m"
  #define COLOR_WHITE   "\033[1;37m"
  #define COLOR_NONE    "\033[0m"

#else

  #define COLOR_BLACK   ""
  #define COLOR_BLUE    ""
  #define COLOR_GREEN   ""
  #define COLOR_CYAN    ""
  #define COLOR_RED     ""
  #define COLOR_PURPLE  ""
  #define COLOR_BROWN   ""
  #define COLOR_LGRAY   ""
  #define COLOR_DGRAY   ""
  #define COLOR_LBLUE   ""
  #define COLOR_LGREEN  ""
  #define COLOR_LCYAN   ""
  #define COLOR_LRED    ""
  #define COLOR_LPURPLE ""
  #define COLOR_YELLOW  ""
  #define COLOR_WHITE   ""
  #define COLOR_NONE    ""

#endif

#define BLACK(s)   (COLOR_BLACK s COLOR_NONE)
#define BLUE(s)    (COLOR_BLUE s COLOR_NONE)
#define GREEN(s)   (COLOR_GREEN s COLOR_NONE)
#define CYAN(s)    (COLOR_CYAN s COLOR_NONE)
#define RED(s)     (COLOR_RED s COLOR_NONE)
#define PURPLE(s)  (COLOR_PURPLE s COLOR_NONE)
#define BROWN(s)   (COLOR_BROWN s COLOR_NONE)
#define LGRAY(s)   (COLOR_LGRAY s COLOR_NONE)
#define DGRAY(s)   (COLOR_DGRAY s COLOR_NONE)
#define LBLUE(s)   (COLOR_LBLUE s COLOR_NONE)
#define LGREEN(s)  (COLOR_LGREEN s COLOR_NONE)
#define LCYAN(s)   (COLOR_LCYAN s COLOR_NONE)
#define LRED(s)    (COLOR_LRED s COLOR_NONE)
#define LPURPLE(s) (COLOR_LPURPLE s COLOR_NONE)
#define YELLOW(s)  (COLOR_YELLOW s COLOR_NONE)
#define WHITE(s)   (COLOR_WHITE s COLOR_NONE)

#define DBG_LVL_NONE    0
#define DBG_LVL_ERROR   1
#define DBG_LVL_WARN    2
#define DBG_LVL_INFO    3
#define DBG_LVL_VERBOSE 4

#ifndef DEBUG
  #define DEBUG DBG_LVL_NONE
#endif

#if DEBUG >= DBG_LVL_ERROR
  #define DBG_ERR(...)   dprintf(1, COLOR_DGRAY "  [%d] ", getpid());\
                         dprintf(1, COLOR_LRED "E:  " COLOR_NONE __VA_ARGS__);
  #define DBG_ERR_N(...) dprintf(1, __VA_ARGS__);
#else
  #define DBG_ERR(...)
  #define DBG_ERR_N(...)
#endif

#if DEBUG >= DBG_LVL_WARN
  #define DBG_WARN(...)   dprintf(1, COLOR_DGRAY "  [%d] ", getpid());\
                          dprintf(1, COLOR_YELLOW "W:  " COLOR_NONE __VA_ARGS__);
  #define DBG_WARN_N(...) dprintf(1, __VA_ARGS__);
#else
  #define DBG_WARN(...)
  #define DBG_WARN_N(...)
#endif

#if DEBUG >= DBG_LVL_INFO
  #define DBG_INFO(...)   dprintf(1, COLOR_DGRAY "  [%d] ", getpid());\
                          dprintf(1, COLOR_LBLUE "I:  "  COLOR_NONE __VA_ARGS__);
  #define DBG_INFO_N(...) dprintf(1, __VA_ARGS__);
#else
  #define DBG_INFO(...)
  #define DBG_INFO_N(...)
#endif

#if DEBUG >= DBG_LVL_VERBOSE
  #define DBG_VERBOSE(...)   dprintf(1, COLOR_DGRAY "  [%d] ", getpid());\
                             dprintf(1, COLOR_DGRAY "V:  "  COLOR_NONE __VA_ARGS__);
  #define DBG_VERBOSE_N(...) dprintf(1, __VA_ARGS__);
#else
  #define DBG_VERBOSE(...)
  #define DBG_VERBOSE_N(...)
#endif

#define DBG_PRINT(...)   dprintf(0, COLOR_DGRAY "  [%d] ", getpid());\
                         dprintf(0, COLOR_CYAN "P:  "  COLOR_NONE __VA_ARGS__);
#define DBG_PRINT_N(...) dprintf(0, __VA_ARGS__);

#define WARN(...)   dprintf(2, COLOR_YELLOW "\u2691     " __VA_ARGS__);\
                    dprintf(2, " (%s:%d)\n", __FILE__, __LINE__);\
                    dprintf(2, COLOR_NONE);

#define PANIC(...)  dprintf(2, COLOR_LRED "\u2717     " __VA_ARGS__);\
                    dprintf(2, " (%s:%d)\n", __FILE__, __LINE__);\
                    dprintf(2, COLOR_NONE);\
                    exit(1);

#define COMPLAIN(...) fprintf(stderr, RED("ERROR: "));\
                      fprintf(stderr, __VA_ARGS__);\
                      fprintf(stderr, "\n");

#endif
