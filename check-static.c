#ifdef NDEBUG
#error Must be compiled without NDEBUG
#endif

#include <assert.h>
#include <stdbool.h>
#include <complex.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define maxsizeof(x, y) max(sizeof(x), sizeof(y))
#define minsizeof(x, y) min(sizeof(x), sizeof(y))

void check_null() {
  assert(__builtin_object_size(NULL, 0) == -1);
}

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

void check_static_malloc() {
  size_t n = 5;
  char *m = malloc(n);
  assert(__builtin_object_size(m, 0) == n);
  assert(__builtin_object_size(m + 0, 0) == n - 0);
  assert(__builtin_object_size(m + 1, 0) == n - 1);
  assert(__builtin_object_size(m + 2, 0) == n - 2);
  assert(__builtin_object_size(m + 3, 0) == n - 3);
  assert(__builtin_object_size(m + 4, 0) == n - 4);
  assert(__builtin_object_size(m + 5, 0) == n - 5);
  free(m);
}

void check_static_calloc() {
  size_t n = 5;
  typedef short elt_t;
  elt_t *m = calloc(n, sizeof(elt_t));
  assert(__builtin_object_size(m, 0) == n * sizeof(elt_t));
  assert(__builtin_object_size(m + 0, 0) == (n - 0) * sizeof(elt_t));
  assert(__builtin_object_size(m + 1, 0) == (n - 1) * sizeof(elt_t));
  assert(__builtin_object_size(m + 2, 0) == (n - 2) * sizeof(elt_t));
  assert(__builtin_object_size(m + 3, 0) == (n - 3) * sizeof(elt_t));
  assert(__builtin_object_size(m + 4, 0) == (n - 4) * sizeof(elt_t));
  assert(__builtin_object_size(m + 5, 0) == (n - 5) * sizeof(elt_t));
  free(m);
}

void check_static_realloc() {
  size_t n = 5;
  char *m = malloc(3);
  m = realloc(m, n);

  assert(__builtin_object_size(m, 0) == n);
  assert(__builtin_object_size(m + 0, 0) == n - 0);
  assert(__builtin_object_size(m + 1, 0) == n - 1);
  assert(__builtin_object_size(m + 2, 0) == n - 2);
  assert(__builtin_object_size(m + 3, 0) == n - 3);
  assert(__builtin_object_size(m + 4, 0) == n - 4);
  assert(__builtin_object_size(m + 5, 0) == n - 5);
  free(m);
}

void check_mixing_malloc() {
  size_t n0 = 5, n1 = 3;
  char *m = rand() & 1 ? malloc(n0) : malloc(n1);

  assert(__builtin_object_size(m, 0) == max(n0, n1));
  assert(__builtin_object_size(m, 1) == max(n0, n1));
  assert(__builtin_object_size(m, 2) == min(n0, n1));

  assert(__builtin_object_size(m + 0, 0) == max(n0, n1) - 0);
  assert(__builtin_object_size(m + 1, 0) == max(n0, n1) - 1);
  assert(__builtin_object_size(m + 2, 0) == max(n0, n1) - 2);
  assert(__builtin_object_size(m + 3, 0) == max(n0, n1) - 3);
  assert(__builtin_object_size(m + 4, 0) == max(n0, n1) - 4);
  assert(__builtin_object_size(m + 5, 0) == max(n0, n1) - 5);
  assert(__builtin_object_size(m + 5, 2) == 0);
  free(m);
}

void check_type_punning() {
  int32_t i;
  short *s = (short*)(char*)&i;
  assert(__builtin_object_size(s, 0) == sizeof(i));
  assert(__builtin_object_size(s + 1, 0) == sizeof(i) - sizeof(*s));

  union { int32_t* ip; short * sp;} u;
  u.ip = &i;
  assert(__builtin_object_size(u.ip, 0) == sizeof(i));
  assert(__builtin_object_size(u.sp, 0) == sizeof(i));
}

void check_copying_address() {
  int32_t i;
  void * v = &i;
  assert(__builtin_object_size(v, 0) == sizeof(i));

  void* addresses[3];
  addresses[1] = v;
  assert(__builtin_object_size(addresses[1], 0) == sizeof(i));

  typedef struct {
    int* v0;
    void* v1;
  } vv_t;
  vv_t vv;
  vv.v1 = addresses[1];
  assert(__builtin_object_size(vv.v1, 0) == sizeof(i));
}

void check_array_of_malloced() {
  void* a[3] = {malloc(1), malloc(2), malloc(3)};

  assert(__builtin_object_size(a[0], 0) == 1);
  assert(__builtin_object_size(a[1], 0) == 2);
  assert(__builtin_object_size(a[2], 0) == 3);

  free(a[0]);
  free(a[1]);
  free(a[2]);
}

int main() {
  check_null();
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
  check_static_malloc();
  check_static_calloc();
  check_static_realloc();
  check_mixing_malloc();
  check_type_punning();
  check_copying_address();
  check_array_of_malloced();
  return 0;
}
