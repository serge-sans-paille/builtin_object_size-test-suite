#include <cstdio>
#include <new>
#include <algorithm>

using namespace std;

static int failures = 0;

#define CHECK(VAL, REF) \
  do { \
    size_t _r = REF;\
    size_t _v = VAL;\
    bool _cond = (_v) == (_r);\
    (void)(_cond || (fprintf(stderr, "[FAIL] %s:%d %s: %s != %s, expecting %zd got %zd\n", __FILE__, __LINE__, __FUNCTION__, #REF, #VAL, _r, _v), ++failures));\
  } while(0)


void check_nullptr() {
  CHECK(__builtin_object_size(nullptr, 0), -1);
}

void check_new() {
  float* a = new float[3];
  CHECK(__builtin_object_size(a, 0), 3 * sizeof(float));
}

void check_placement_new() {
  double d;
  double* mem = new (&d) double ();
  CHECK(__builtin_object_size(mem, 0), sizeof(double));
}

void check_tls() {
  thread_local int i;
  CHECK(__builtin_object_size(&i, 0), sizeof(int));
}

static int buffer[2];
void check_exception() {
  int * mem;
  try {
    mem = new int(3);
  }
  catch(...) {
    mem = buffer;
  };
  CHECK(__builtin_object_size(&mem, 0), sizeof(int) * std::max(2, 3));
  CHECK(__builtin_object_size(&mem, 2), sizeof(int) * std::min(2, 3));
}

int main() {
  check_nullptr();
  check_new();
  check_placement_new();
  check_tls();
  return failures;
}
