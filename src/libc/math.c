#include <libc/math.h>

s64 abs(s64 val) {
  return val < 0 ? -val : val;
}
