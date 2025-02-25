#include <libc/math.h>

u64 abs(u64 val) {
  return val < 0 ? -val : val;
}
