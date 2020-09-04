#include "sfc/test.h"

#pragma comment(lib, "pthread")

using namespace sfc;

int main(int argc, const char* argv[]) {
  vec::Vec<Str> args;
  for (int i = 1; i < argc; ++i) {
    args.push(Str::from_cstr(argv[i]));
  }
  test::manager().run({args.as_slice()});
  return 0;
}
