#include "sfc/test.h"

#include "sfc/collections/linked_list.h"

namespace sfc::collections::linked_list {

#if 0
sfc_test(push) {
  auto q = LinkedList<i32>{};
  q.push_front(2);
  q.push_front(1);
  q.push_front(0);

  q.push_back(3);
  q.push_back(4);
  q.push_back(5);

  log::info("q = {}", q);
}
#endif

}  // namespace sfc::collections::linked_list
