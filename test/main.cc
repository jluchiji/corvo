#include <cstdio>
#include <unistd.h>
#include "include/transpose/buffer.h"
#include "include/transpose/fragment.h"

int main() {

  Fragment *root = new Fragment("<ul>{{child}}</ul>");
  Fragment *child = new Fragment("Hello <b>{{name}}</b>!");

  child -> set("name", new Fragment("Anton"));
  root -> set("child", child);

  Buffer *result = root -> render();
  write(1, result -> data(), result -> length());
}
