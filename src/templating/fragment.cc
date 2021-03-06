// ------------------------------------------------------------------------- //
//                                                                           //
// CS252 Lab05 - HTTP Server                                                 //
// Copyright © 2015 Денис Лучкин-Чжоу                                        //
//                                                                           //
// fragment.cc                                                                //
// Данный файл содержит реализацию класса Fragment, который отвечает за      //
// динамичное составление текста.                                            //
//                                                                           //
// ------------------------------------------------------------------------- //
#include <string.h>

#include "fragment.h"
#include "io/buffer.h"
#include "trace.h"

Fragment::Fragment(const void *tmpl, size_t n) {
  this -> tmpl = new char[n + 1];
  memcpy(this -> tmpl, tmpl, n);
  this -> tmpl[n] = 0;
}

// ------------------------------------------------------------------------- //
// Конструктор. Создает объект из текста-шаблона.                            //
// ------------------------------------------------------------------------- //
Fragment::Fragment(const char *tmpl) {
  this -> tmpl = strdup(tmpl);
}

// ------------------------------------------------------------------------- //
// Деструктор. Уничтожает объект.                                            //
// ------------------------------------------------------------------------- //
Fragment::~Fragment() {

  /* Удаляем текст шаблона */
  delete tmpl;

  /* Удаляем фрагменты для вставки */
  FragmentMap::iterator it = params.begin();
  for (; it != params.end(); ++it) {
    delete it -> first;
    delete it -> second;
  }
}

// ------------------------------------------------------------------------- //
// Связывает идентификатор с фрагментом для вставки.                         //
// ------------------------------------------------------------------------- //
void
Fragment::set(const char *name, Fragment *value) {
  /* Avoid redundant duplication */
  if (value) {
    params[strdup(name)] = value;
  }
  /* NULL value means erase */
  else {
    params.erase(name);
  }
}

// ------------------------------------------------------------------------- //
// Связывает идентификатор с текстом для вставки.                            //
// ------------------------------------------------------------------------- //
void
Fragment::set(const char *name, const char *value) {
  Fragment *child = new Fragment(value);
  this -> set(name, child);
}

// ------------------------------------------------------------------------- //
// Создает пустой буфер и вставляет фрагмент.                                //
// ------------------------------------------------------------------------- //
Buffer*
Fragment::render() {
  Buffer *buffer = new Buffer();
  this -> render(buffer);
  buffer -> write("\0", 1); // NULL
  return buffer;
}

// ------------------------------------------------------------------------- //
// Вставляет фрагмент в имеющийся буфер.                                     //
// ------------------------------------------------------------------------- //
void
Fragment::render(Buffer *buffer) {
  char  *po = tmpl,
        *ps = po,
        *pe;
  /* Поиск идентификаторов */
  while ((ps = strstr(ps, "{{"))) {
    /* Вставить текст перед спедующим идентификатором */
    buffer -> write(po, ps - po);
    po = ps;
    /* Поиск конца идентификатора */
    if (!(pe = strstr(ps, "}}"))) { break; }
    /* Поиск фрагмента для вставки */
    char *name = strndup(ps + 2, pe - ps - 2);
    po = ps = pe + 2;
    Fragment *child = params[name];
    /* Если фрагмент для указанного идентификатора не существует */
    if (!child) {
      buffer -> write("{{", 2);
      buffer -> write(name, strlen(name));
      buffer -> write("}}", 2);
      continue;
    }
    /* Вставка фрагмента в буфер */
    child -> render(buffer);
    /* Уборка памяти */
    delete name;
  }
  /* Вставка оставшегося текста */
  buffer -> write(po, strlen(po));
}
