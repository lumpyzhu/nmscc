#if 0
#include "sfc/collections/btree.h"

#include "sfc/test.h"

namespace sfc::collections::btree {

sfc_test(BTreeMapI32) {
  auto map = BTreeMap<i32, f32>::xnew();
  map.insert(1, 1.1f);
  map.insert(2, 2.2f);

  log::info("{} -> {}", 1, map.get(1));
  log::info("{} -> {}", 2, map.get(2));
}

sfc_test(BTreeMapString) {
  auto map = BTreeMap<String, String>::xnew();
  map.insert(String::from_str("a"), String::from_str("A"));
  map.insert(String::from_str("b"), String::from_str("B"));

  auto a = map.get("a");
  auto b = map.get("b");

  log::info("a = {}", a);
  log::info("b = {}", b);
}

}  // namespace sfc::collections::btree
#endif
