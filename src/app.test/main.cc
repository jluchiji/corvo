#include <cstdio>
#include <unistd.h>

#include "include/transpose/buffer.h"
#include "include/transpose/fragment.h"
#include "include/global.h"

#include "embed/error.html.h"
#include "embed/styles.css.h"
#include "embed/pure.css.h"

int main() {
  Fragment *fragment = new Fragment(error_html, error_html_len);
  fragment -> set("status-code", "404");
  fragment -> set("status-message", "Not Found");
  fragment -> set("css-pure", new Fragment(pure_css, pure_css_len));
  fragment -> set("css-style", new Fragment(styles_css, styles_css_len));
  Buffer *buffer = fragment -> render();
  write(1, buffer -> data(), buffer -> length());
}
