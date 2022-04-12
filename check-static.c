#include <stdbool.h>
#include <complex.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <alloca.h>

#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define maxsizeof(x, y) max(sizeof(x), sizeof(y))
#define minsizeof(x, y) min(sizeof(x), sizeof(y))

static int failures = 0;

#define CHECK(VAL, REF) \
  do { \
    size_t _r = REF;\
    size_t _v = VAL;\
    bool _cond = (_v) == (_r);\
    (void)(_cond || (fprintf(stderr, "[FAIL] %s:%d %s: %s != %s, expecting %zd got %zd\n", __FILE__, __LINE__, __FUNCTION__, #REF, #VAL, _r, _v), ++failures));\
  } while(0)


void check_null() {
  CHECK(__builtin_object_size(NULL, 0), -1);
}

void* my_calloc(size_t nmemb, size_t size) __attribute__((alloc_size(1,2))) __attribute__((noinline));
void* my_calloc(size_t nmemb, size_t size) {
  return calloc(nmemb, size);
}

void* my_malloc(size_t size) __attribute__((alloc_size(1))) __attribute__((noinline));
void* my_malloc(size_t size) {
  return malloc(size);
}

void check_alloc_size_attribute() {
  int n = 3, p = 7;
  void* c = my_calloc(n, p);
  CHECK(__builtin_object_size(c, 0), n * p);
  free(c);

  void* m = my_malloc(n);
  CHECK(__builtin_object_size(m, 0), n);
  free(m);
}

void check_sideeffects() {
  int a[8];
  void* p = a;
  CHECK(__builtin_object_size(p++, 0), -1);
  CHECK(__builtin_object_size(p++, 1), -1);
  CHECK(__builtin_object_size(p++, 2), 0);
}


void check_conditionnaly_null() {
  void * p = NULL;
  if(rand() & 1)
    p = &p;
  CHECK(__builtin_object_size(p, 0), -1);
  CHECK(__builtin_object_size(p, 1), -1);
  CHECK(__builtin_object_size(p, 2), 0);
  CHECK(__builtin_object_size(p, 3), 0);
}


void check_integers() {
  bool b;
  CHECK(__builtin_object_size(&b, 0), sizeof(b));
  char c;
  CHECK(__builtin_object_size(&c, 0), sizeof(c));
  short s;
  CHECK(__builtin_object_size(&s, 0), sizeof(s));
  int i;
  CHECK(__builtin_object_size(&i, 0), sizeof(i));
  long l;
  CHECK(__builtin_object_size(&l, 0), sizeof(l));
  long long ll;
  CHECK(__builtin_object_size(&ll, 0), sizeof(ll));
}

void check_floats() {
  float f;
  CHECK(__builtin_object_size(&f, 0), sizeof(f));
  double d;
  CHECK(__builtin_object_size(&d, 0), sizeof(d));
  long double ld;
  CHECK(__builtin_object_size(&ld, 0), sizeof(ld));
}

void check_complex() {
  float _Complex fc;
  CHECK(__builtin_object_size(&fc, 0), sizeof(fc));
  double _Complex dc;
  CHECK(__builtin_object_size(&dc, 0), sizeof(dc));
  long double _Complex ldc;
  CHECK(__builtin_object_size(&ldc, 0), sizeof(ldc));
}

void check_simple_array() {
  int a[3];
  CHECK(__builtin_object_size(a, 0), sizeof(a));
  CHECK(__builtin_object_size(&a[0], 0), sizeof(a));
  CHECK(__builtin_object_size(&a[1], 0), sizeof(a[0]) * 2);
  CHECK(__builtin_object_size(&a[2], 0), sizeof(a[0]) * 1);
  CHECK(__builtin_object_size(&a[3], 0), sizeof(a[0]) * 0);
}

int a[3];
void check_global_array() {
  CHECK(__builtin_object_size(a, 0), sizeof(a));
  CHECK(__builtin_object_size(&a[0], 0), sizeof(a));
  CHECK(__builtin_object_size(&a[1], 0), sizeof(a[0]) * 2);
  CHECK(__builtin_object_size(&a[2], 0), sizeof(a[0]) * 1);
  CHECK(__builtin_object_size(&a[3], 0), sizeof(a[0]) * 0);
}

int a[3];
void check_global_array_with_offset() {
  CHECK(__builtin_object_size(a + 2, 0), sizeof(a) - 2 * sizeof(int));
}

void check_simple_struct() {
  typedef struct {
    float f;
    int i;
  } fi_t;
  fi_t fi;

  CHECK(__builtin_object_size(&fi, 0), sizeof(fi));
  CHECK(__builtin_object_size(&fi.f, 0), sizeof(fi) - offsetof(fi_t, f));
  CHECK(__builtin_object_size(&fi.i, 0), sizeof(fi) - offsetof(fi_t, i));
}

void check_simple_union() {
  typedef union {
    float f;
    short s;
  } fs_t;
  fs_t fs;

  CHECK(__builtin_object_size(&fs, 0), sizeof(fs));
  CHECK(__builtin_object_size(&fs.f, 0), sizeof(fs) - offsetof(fs_t, f));
  CHECK(__builtin_object_size(&fs.s, 0), sizeof(fs) - offsetof(fs_t, s));
}

void check_padded_struct() {
  typedef struct {
    char c;
    int i;
  } ci_t;
  ci_t ci;

  CHECK(__builtin_object_size(&ci, 0), sizeof(ci));
  CHECK(__builtin_object_size(&ci.c, 0), sizeof(ci) - offsetof(ci_t, c));
  CHECK(__builtin_object_size(&ci.i, 0), sizeof(ci) - offsetof(ci_t, i));
}

void check_struct_with_aligned_field() {
  typedef struct {
    char c;
    _Alignas(float) char f;
  } cf_t;
  cf_t cf;

  CHECK(__builtin_object_size(&cf, 0), sizeof(cf));
  CHECK(__builtin_object_size(&cf.c, 0), sizeof(cf) - offsetof(cf_t, c));
  CHECK(__builtin_object_size(&cf.f, 0), sizeof(cf) - offsetof(cf_t, f));
}

void check_array_of_struct() {
  typedef struct {
    short s;
    double d;
  } sd_t;
  sd_t a[3];
  CHECK(__builtin_object_size(a, 0), sizeof(a));
  CHECK(__builtin_object_size(&a[0], 0), sizeof(a));
  CHECK(__builtin_object_size(&a[0].s, 0), sizeof(a) - offsetof(sd_t, s));
  CHECK(__builtin_object_size(&a[0].d, 0), sizeof(a) - offsetof(sd_t, d));
  CHECK(__builtin_object_size(&a[1], 0), sizeof(a[0]) * 2);
  CHECK(__builtin_object_size(&a[2], 0), sizeof(a[0]) * 1);
  CHECK(__builtin_object_size(&a[3], 0), sizeof(a[0]) * 0);
}


void check_ifexpr_scalars() {
  float f0, f1; double d;
  void* ff = rand() & 1 ? &f0 : &f1;
  CHECK(__builtin_object_size(ff, 0), maxsizeof(f0, f1));

  void* fd = rand() & 1 ? (void*)&f0 : (void*)&d;
  CHECK(__builtin_object_size(fd, 0), maxsizeof(f0, d));
  CHECK(__builtin_object_size(fd, 1), maxsizeof(f0, d));
  CHECK(__builtin_object_size(fd, 2), minsizeof(f0, d));
}

void check_if_scalars() {
  float f0, f1; double d;
  void* ff = &f0;
  if(rand() & 1)
    ff = &f1;
  CHECK(__builtin_object_size(ff, 0), maxsizeof(f0, f1));

  void* fd;
  if(rand() & 1)
    fd = &f0;
  else
    fd = &d;
  CHECK(__builtin_object_size(fd, 0), maxsizeof(f0, d));
  CHECK(__builtin_object_size(fd, 1), maxsizeof(f0, d));
  CHECK(__builtin_object_size(fd, 2), minsizeof(f0, d));
}

void check_if_then_offset() {
  float f0, f1; double d;
  void* ff = &f0;
  if(rand() & 1)
    ff = &f1;
  CHECK(__builtin_object_size(ff + 1, 0), maxsizeof(f0, f1) - 1);

  void* fd;
  if(rand() & 1)
    fd = &f0;
  else
    fd = &d;
  CHECK(__builtin_object_size(fd + 1, 0), maxsizeof(f0, d) - 1);
  CHECK(__builtin_object_size(fd + 1, 1), maxsizeof(f0, d) - 1);
  CHECK(__builtin_object_size(fd + 1, 2), minsizeof(f0, d) - 1);
}

void check_while_scalars() {
  float f; double d;
  void* fd = &f;
  while(rand() & 1)
    fd = &d;
  CHECK(__builtin_object_size(fd, 0), maxsizeof(f, d));
  CHECK(__builtin_object_size(fd, 1), maxsizeof(f, d));
  CHECK(__builtin_object_size(fd, 2), minsizeof(f, d));
}

void check_ifexpr_arrays() {
  float a2[2], a3[3];

  void* a = rand() & 1 ? (void*)a2 : (void*)a3;
  CHECK(__builtin_object_size(a, 0), maxsizeof(a2, a3));
  CHECK(__builtin_object_size(a, 1), maxsizeof(a2, a3));
  CHECK(__builtin_object_size(a, 2), minsizeof(a2, a3));

  void* a1 = rand() & 1 ? (void*)&a2[1] : (void*)&a3[1];
  CHECK(__builtin_object_size(a1, 0), maxsizeof(a2, a3) - sizeof(a2[0]));
  CHECK(__builtin_object_size(a1, 1), maxsizeof(a2, a3) - sizeof(a2[0]));
  CHECK(__builtin_object_size(a1, 2), minsizeof(a2, a3) - sizeof(a2[0]));
}

void check_static_alloca() {
  size_t n = 5;
  char *m = alloca(n);
  CHECK(__builtin_object_size(m, 0), n);
  CHECK(__builtin_object_size(m + 0, 0), n - 0);
  CHECK(__builtin_object_size(m + 1, 0), n - 1);
  CHECK(__builtin_object_size(m + 2, 0), n - 2);
  CHECK(__builtin_object_size(m + 3, 0), n - 3);
  CHECK(__builtin_object_size(m + 4, 0), n - 4);
  CHECK(__builtin_object_size(m + 5, 0), n - 5);
}

void check_static_malloc() {
  size_t n = 5;
  char *m = malloc(n);
  CHECK(__builtin_object_size(m, 0), n);
  CHECK(__builtin_object_size(m + 0, 0), n - 0);
  CHECK(__builtin_object_size(m + 1, 0), n - 1);
  CHECK(__builtin_object_size(m + 2, 0), n - 2);
  CHECK(__builtin_object_size(m + 3, 0), n - 3);
  CHECK(__builtin_object_size(m + 4, 0), n - 4);
  CHECK(__builtin_object_size(m + 5, 0), n - 5);
  free(m);
}

void check_freed_memory() {
  size_t n = 5;
  char *m = malloc(n);
  free(m);
  CHECK(__builtin_object_size(m, 0), -1);
}

void check_static_calloc() {
  size_t n = 5;
  typedef short elt_t;
  elt_t *m = calloc(n, sizeof(elt_t));
  CHECK(__builtin_object_size(m, 0), n * sizeof(elt_t));
  CHECK(__builtin_object_size(m + 0, 0), (n - 0) * sizeof(elt_t));
  CHECK(__builtin_object_size(m + 1, 0), (n - 1) * sizeof(elt_t));
  CHECK(__builtin_object_size(m + 2, 0), (n - 2) * sizeof(elt_t));
  CHECK(__builtin_object_size(m + 3, 0), (n - 3) * sizeof(elt_t));
  CHECK(__builtin_object_size(m + 4, 0), (n - 4) * sizeof(elt_t));
  CHECK(__builtin_object_size(m + 5, 0), (n - 5) * sizeof(elt_t));
  free(m);
}

void check_static_realloc() {
  size_t n = 5;
  char *m = malloc(3);
  m = realloc(m, n);

  CHECK(__builtin_object_size(m, 0), n);
  CHECK(__builtin_object_size(m + 0, 0), n - 0);
  CHECK(__builtin_object_size(m + 1, 0), n - 1);
  CHECK(__builtin_object_size(m + 2, 0), n - 2);
  CHECK(__builtin_object_size(m + 3, 0), n - 3);
  CHECK(__builtin_object_size(m + 4, 0), n - 4);
  CHECK(__builtin_object_size(m + 5, 0), n - 5);
  free(m);
}

void check_mixing_malloc() {
  size_t n0 = 5, n1 = 3;
  char *m = rand() & 1 ? malloc(n0) : malloc(n1);

  CHECK(__builtin_object_size(m, 0), max(n0, n1));
  CHECK(__builtin_object_size(m, 1), max(n0, n1));
  CHECK(__builtin_object_size(m, 2), min(n0, n1));

  CHECK(__builtin_object_size(m + 0, 0), max(n0, n1) - 0);
  CHECK(__builtin_object_size(m + 1, 0), max(n0, n1) - 1);
  CHECK(__builtin_object_size(m + 2, 0), max(n0, n1) - 2);
  CHECK(__builtin_object_size(m + 3, 0), max(n0, n1) - 3);
  CHECK(__builtin_object_size(m + 4, 0), max(n0, n1) - 4);
  CHECK(__builtin_object_size(m + 5, 0), max(n0, n1) - 5);
  CHECK(__builtin_object_size(m + 5, 2), 0);
  free(m);
}

void check_type_punning() {
  int32_t i;
  short *s = (short*)(char*)&i;
  CHECK(__builtin_object_size(s, 0), sizeof(i));
  CHECK(__builtin_object_size(s + 1, 0), sizeof(i) - sizeof(*s));

  union { int32_t* ip; short * sp;} u;
  u.ip = &i;
  CHECK(__builtin_object_size(u.ip, 0), sizeof(i));
  CHECK(__builtin_object_size(u.sp, 0), sizeof(i));
}

void check_static_posix_memalign() {
  void* m;
  size_t n = 7;
  if(posix_memalign(&m, 8, n))
    return;
  CHECK(__builtin_object_size(m, 0), n);

  m = aligned_alloc(8, n);
  CHECK(__builtin_object_size(m, 0), n);
}


void check_trailing_array_idiom() {
  int n = 10;
  typedef struct {
    int head;
    char tail[];
  } ht_t;
  ht_t * data = malloc(sizeof(ht_t) + n);
  CHECK(__builtin_object_size(data, 0), sizeof(ht_t) + n);
  CHECK(__builtin_object_size(&data->head, 0), sizeof(ht_t) + n);
  CHECK(__builtin_object_size(&data->tail[0], 0), sizeof(ht_t) + n - offsetof(ht_t, tail));
}

void check_strdup() {
  static char const s[] = "erty";
  CHECK(__builtin_object_size(s, 0), sizeof(s));
  char * st = strdup(s);
  CHECK(__builtin_object_size(st, 0), sizeof(s));
  free(st);
}

void check_strndup() {
  static char const s[] = "erty";
  CHECK(__builtin_object_size(s, 0), sizeof(s));
  char * st = strndup(s, sizeof(s));
  CHECK(__builtin_object_size(st, 0), sizeof(s));
  free(st);

  int n = 3;
  st = strndup(s, n);
  CHECK(__builtin_object_size(st, 0), n + 1);
}

void check_copying_address() {
  int32_t i;
  void * v = &i;
  CHECK(__builtin_object_size(v, 0), sizeof(i));

  void* addresses[3];
  addresses[1] = v;
  CHECK(__builtin_object_size(addresses[1], 0), sizeof(i));

  typedef struct {
    int* v0;
    void* v1;
  } vv_t;
  vv_t vv;
  vv.v1 = addresses[1];
  CHECK(__builtin_object_size(vv.v1, 0), sizeof(i));
}

void check_array_of_malloced() {
  void* a[3] = {malloc(1), malloc(2), malloc(3)};

  CHECK(__builtin_object_size(a[0], 0), 1);
  CHECK(__builtin_object_size(a[1], 0), 2);
  CHECK(__builtin_object_size(a[2], 0), 3);

  free(a[0]);
  free(a[1]);
  free(a[2]);
}

void check_storing_in_arg(void** restrict mem) {
  *mem = malloc(5);
  CHECK(__builtin_object_size(*mem, 0), 5);
}
void check_storing_in_field() {
  struct dummy {
    void * mem;
  } dat = { malloc(5) };

  CHECK(__builtin_object_size(dat.mem, 0), 5);

  free(dat.mem);
}

void check_conditional_storing_in_arg(void** restrict mem) {
  int n0 = 5, n1 = 8;
  *mem = malloc(n0);
  if(rand() & 1) {
    free(*mem);
    *mem = malloc(n1);
  }
  CHECK(__builtin_object_size(*mem, 0), max(n0, n1));
  CHECK(__builtin_object_size(*mem, 1), max(n0, n1));
  CHECK(__builtin_object_size(*mem, 2), min(n0, n1));
}

struct dummy {
  void * mem;
};
void check_storing_in_arg_field(struct dummy* restrict dat) {
  dat->mem = malloc(5);

  CHECK(__builtin_object_size(dat->mem, 0), 5);

  free(dat->mem);
}

void check_type_erased_memory() {
  float f;
  uintptr_t i = (uintptr_t)&f;
  void * v = (void*)i;
  CHECK(__builtin_object_size(v, 0), sizeof(float));
}

void check_strcpy() {
  char buffer[8];
  char* s = strcpy(buffer, "hi");
  CHECK(__builtin_object_size(s, 0), sizeof(buffer));
}

int main() {
  check_null();
  check_sideeffects();
  check_conditionnaly_null();
  check_integers();
  check_floats();
  check_complex();
  check_simple_array();
  check_global_array();
  check_global_array_with_offset();
  check_simple_struct();
  check_simple_union();
  check_array_of_struct();
  check_padded_struct();
  check_struct_with_aligned_field();
  check_ifexpr_scalars();
  check_if_scalars();
  check_if_then_offset();
  check_while_scalars();
  check_ifexpr_arrays();
  check_static_alloca();
  check_static_malloc();
  // what's the official status of bos on freeed memory o_O
  //check_freed_memory();
  check_static_calloc();
  check_static_realloc();
  check_static_posix_memalign();
  check_mixing_malloc();
  check_type_punning();
  check_copying_address();
  check_array_of_malloced();
  check_trailing_array_idiom();
  check_strdup();
  check_strndup();
  check_alloc_size_attribute();
  void* mem;
  check_storing_in_arg(&mem);
  free(mem);
  check_storing_in_field();
  struct dummy dat;
  check_storing_in_arg_field(&dat);
  check_conditional_storing_in_arg(&mem);
  check_type_erased_memory();
  check_strcpy();
  return failures;
}
