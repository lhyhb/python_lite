/* The MIT License

   Copyright (c) 2008, by Attractive Chaos <attractor@live.co.uk>

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
   */

/*
  An example:

  #include "kvec.h"
  int main() {
  kvec_t(int) array;
  kv_init(array);
  kv_push(int, array, 10); // append
  kv_a(int, array, 20) = 5; // dynamic
  kv_A(array, 20) = 4; // static
  kv_destroy(array);
  return 0;
  }
  */

/*
  2008-09-22 (0.1.0):

  * The initial version.

  */

#ifndef AC_KVEC_H
#define AC_KVEC_H

#include <stdlib.h>
#include "../utils/config.h"

#ifndef krealloc
#define krealloc(I,P,O,Z) pylt_realloc(I,P,O,Z)
#endif
#ifndef kfree
#define kfree(I,P,S) pylt_free(I, P, S)
#define kfree_ex(I,P) pylt_free_ex(I, P)
#endif

#define kv_roundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))

#define kvec_t(type) struct { size_t n, m; type *a;PyLiteInterpreter *I; }
#define kv_init(I, v) ((v).n = (v).m = 0, (v).a = 0, (v).I = I)
#define kv_destroy(v) kfree((v).I, (v).a, sizeof(*((v).a)) * (v).m)
#define kv_A(v, i) ((v).a[(i)])
#define kv_top(v) ((v).a[(v).n-1])
#define kv_topn(v, num) ((v).a[(v).n-1-(num)])
#define kv_pop(v) ((v).a[--(v).n])
#define kv_popn(v, num) (v).n -= (num)
#define kv_size(v) ((v).n)
#define kv_max(v) ((v).m)
#define kv_clear(v) ((v).n = 0)

#define kv_resize(type, v, s)  ((v).a = (type*)krealloc((v).I, (v).a, sizeof(type) * (v).m, sizeof(type) * (s)), (v).m = (s))

#define kv_shallowcopy(dest, src) { \
    (dest).n = (src).n; (dest).m = (src).m; (dest).a = (src).a; (dest).I = (src).I; \
}

#define kv_copy(type, v1, v0) do {                            \
        if ((v1).m < (v0).n) kv_resize(type, v1, (v0).n);    \
        (v1).n = (v0).n;                                    \
        memcpy((v1).a, (v0).a, sizeof(type) * (v0).n);        \
    } while (0)                                                \

#define kv_copy1(type, v1, v0) do {                            \
        if ((v1).m < (v0).n) kv_resize(type, v1, (v0).n+1);    \
        (v1).n = (v0).n;                                    \
        memcpy((v1).a, (v0).a, sizeof(type) * (v0).n);        \
        } while (0)                                                \

#define kv_push(type, v, x) do {                                    \
        if ((v).n == (v).m) {                                        \
            (v).m = (v).m? (v).m<<1 : 2;                            \
            (v).a = (type*)krealloc((v).I, (v).a, sizeof(type) * (v).n, sizeof(type) * (v).m);    \
        }                                                            \
        (v).a[(v).n++] = (x);                                        \
    } while (0)

#define kv_pushp(type, v) (((v).n == (v).m)?                            \
                           ((v).m = ((v).m? (v).m<<1 : 2),                \
                            (v).a = (type*)krealloc((v).I, (v).a, sizeof(type) * (v).n, sizeof(type) * (v).m), 0)    \
                           : 0), ((v).a + ((v).n++))

#endif
