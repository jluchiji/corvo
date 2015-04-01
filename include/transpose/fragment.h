// ------------------------------------------------------------------------- //
//                                                                           //
// CS252 Lab05 - HTTP Server                                                 //
// Copyright © 2015 Денис Лучкин-Чжоу                                        //
//                                                                           //
// fragment.h                                                                //
// Данный файл содержит объявление класса Fragment, который отвечает за      //
// динамичное составление текста.                                            //
//                                                                           //
// ------------------------------------------------------------------------- //
#ifndef _FRAGMENT_H_
#define _FRAGMENT_H_

#include <cstdlib>
#include <map>
#include "include/transpose/buffer.h"

/* Опережающее объявление */
class Fragment;

/* Псевдонимы видов */
struct StringComp :
  public std::binary_function<const char*, const char*, bool> {
    bool operator()(char const *a, char const *b) const {
      return strcasecmp(a, b) < 0;
    }
};
typedef std::pair<const char*, Fragment*> FragmentPair;
typedef std::map<const char*, Fragment*, StringComp> FragmentMap;

// ------------------------------------------------------------------------- //
// Класс Fragment: ...それと便座カバー                                         //
// ------------------------------------------------------------------------- //
class Fragment {
private:
  char          *tmpl;                   // Шаблон
  FragmentMap    params;                 // Фрагменты для вставки

public:
  Fragment(const char*);                 // Создает фрагмент из шаблона
  ~Fragment();                           // Уничтожает объект

  void    set(const char*, Fragment*);   // Добавляет фрагмент для вставки
  void    set(const char*, const char*); // Добавляет текст для вставки
  Buffer* render();                      // Вставляет фрагмент в пустой буфер
  void    render(Buffer*);               // Вставляет фрагмент в буфер
};

#endif
