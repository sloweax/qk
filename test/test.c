#include <stdlib.h>
#include "unit.h"
#include "test_hmap.h"
#include "test_ll.h"
#include "test_buf.h"
#include "test_eval.h"

int main()
{
    atexit(result);
    test_hmap();
    test_ll();
    test_buf();
    test_buf_str();
    test_eval();
}
