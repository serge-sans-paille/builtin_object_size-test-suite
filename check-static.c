#ifdef NDEBUG
#error Must be compiled without NDEBUG
#endif

#include <assert.h>
#include <stdbool.h>
#include <complex.h>
#include <stdlib.h>
#include <stddef.h>

#define maxsizeof(x, y) (sizeof(x) > sizeof(y) ? sizeof(x) : sizeof(y))
#define minsizeof(x, y) (sizeof(x) < sizeof(y) ? sizeof(x) : sizeof(y))

void check_integers() {
  bool b;
  assert(__builtin_object_size(&b, 0) == sizeof(b));
  char c;
  assert(__builtin_object_size(&c, 0) == sizeof(c));
  short s;
  assert(__builtin_object_size(&s, 0) == sizeof(s));
  int i;
  assert(__builtin_object_size(&i, 0) == sizeof(i));
  long l;
  assert(__builtin_object_size(&l, 0) == sizeof(l));
  long long ll;
  assert(__builtin_object_size(&ll, 0) == sizeof(ll));
}

void check_floats() {
  float f;
  assert(__builtin_object_size(&f, 0) == sizeof(f));
  double d;
  assert(__builtin_object_size(&d, 0) == sizeof(d));
  long double ld;
  assert(__builtin_object_size(&ld, 0) == sizeof(ld));
}

void check_complex() {
  float _Complex fc;
  assert(__builtin_object_size(&fc, 0) == sizeof(fc));
  double _Complex dc;
  assert(__builtin_object_size(&dc, 0) == sizeof(dc));
  long double _Complex ldc;
  assert(__builtin_object_size(&ldc, 0) == sizeof(ldc));
}

void check_simple_array() {
  int a[3];
  assert(__builtin_object_size(a, 0) == sizeof(a));
  assert(__builtin_object_size(&a[0], 0) == sizeof(a));
  assert(__builtin_object_size(&a[1], 0) == sizeof(a[0]) * 2);
  assert(__builtin_object_size(&a[2], 0) == sizeof(a[0]) * 1);
  assert(__builtin_object_size(&a[3], 0) == sizeof(a[0]) * 0);
}

void check_simple_struct() {
  typedef struct {
    float f;
    int i;
  } fi_t;
  fi_t fi;

  assert(__builtin_object_size(&fi, 0) == sizeof(fi));
  assert(__builtin_object_size(&fi.f, 0) == sizeof(fi) - offsetof(fi_t, f));
  assert(__builtin_object_size(&fi.i, 0) == sizeof(fi) - offsetof(fi_t, i));
}

void check_simple_union() {
  typedef union {
    float f;
    short s;
  } fs_t;
  fs_t fs;

  assert(__builtin_object_size(&fs, 0) == sizeof(fs));
  assert(__builtin_object_size(&fs.f, 0) == sizeof(fs) - offsetof(fs_t, f));
  assert(__builtin_object_size(&fs.s, 0) == sizeof(fs) - offsetof(fs_t, s));
}

void check_padded_struct() {
  typedef struct {
    char c;
    int i;
  } ci_t;
  ci_t ci;

  assert(__builtin_object_size(&ci, 0) == sizeof(ci));
  assert(__builtin_object_size(&ci.c, 0) == sizeof(ci) - offsetof(ci_t, c));
  assert(__builtin_object_size(&ci.i, 0) == sizeof(ci) - offsetof(ci_t, i));
}

void check_struct_with_aligned_field() {
  typedef struct {
    char c;
    _Alignas(float) char f;
  } cf_t;
  cf_t cf;

  assert(__builtin_object_size(&cf, 0) == sizeof(cf));
  assert(__builtin_object_size(&cf.c, 0) == sizeof(cf) - offsetof(cf_t, c));
  assert(__builtin_object_size(&cf.f, 0) == sizeof(cf) - offsetof(cf_t, f));
}

void check_array_of_struct() {
  typedef struct {
    short s;
    double d;
  } sd_t;
  sd_t a[3];
  assert(__builtin_object_size(a, 0) == sizeof(a));
  assert(__builtin_object_size(&a[0], 0) == sizeof(a));
  assert(__builtin_object_size(&a[0].s, 0) == sizeof(a) - offsetof(sd_t, s));
  assert(__builtin_object_size(&a[0].d, 0) == sizeof(a) - offsetof(sd_t, d));
  assert(__builtin_object_size(&a[1], 0) == sizeof(a[0]) * 2);
  assert(__builtin_object_size(&a[2], 0) == sizeof(a[0]) * 1);
  assert(__builtin_object_size(&a[3], 0) == sizeof(a[0]) * 0);
}


void check_ifexpr_scalars() {
  float f0, f1; double d;
  void* ff = rand() & 1 ? &f0 : &f1;
  assert(__builtin_object_size(ff, 0) == maxsizeof(f0, f1));

  void* fd = rand() & 1 ? (void*)&f0 : (void*)&d;
  assert(__builtin_object_size(fd, 0) == maxsizeof(f0, d));
  assert(__builtin_object_size(fd, 1) == maxsizeof(f0, d));
  assert(__builtin_object_size(fd, 2) == minsizeof(f0, d));
}

void check_ifexpr_arrays() {
  float a2[2], a3[3];

  void* a = rand() & 1 ? (void*)a2 : (void*)a3;
  assert(__builtin_object_size(a, 0) == maxsizeof(a2, a3));
  assert(__builtin_object_size(a, 1) == maxsizeof(a2, a3));
  assert(__builtin_object_size(a, 2) == minsizeof(a2, a3));

  void* a1 = rand() & 1 ? (void*)&a2[1] : (void*)&a3[1];
  assert(__builtin_object_size(a1, 0) == maxsizeof(a2, a3) - sizeof(a2[0]));
  assert(__builtin_object_size(a1, 1) == maxsizeof(a2, a3) - sizeof(a2[0]));
  assert(__builtin_object_size(a1, 2) == minsizeof(a2, a3) - sizeof(a2[0]));
}

int main() {
  check_integers();
  check_floats();
  check_complex();
  check_simple_array();
  check_simple_struct();
  check_simple_union();
  check_array_of_struct();
  check_padded_struct();
  check_struct_with_aligned_field();
  check_ifexpr_scalars();
  check_ifexpr_arrays();
  return 0;
}
