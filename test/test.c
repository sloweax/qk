#include <stdlib.h>
#include "unit.h"
#include "test_hmap.h"
#include "test_int.h"
#include "test_ll.h"
#include "test_buf.h"

int main()
{
    atexit(result);
    test_hmap();
    test_int();
    test_ll();
    test_buf();
    test_buf_str();
}
