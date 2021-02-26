#include "acutest.h" // https://github.com/mity/acutest
#include "hpy/debug/src/debug_internal.h"

void test_DHQueue_init(void)
{
    DHQueue q;
    DHQueue_init(&q);
    TEST_CHECK(q.head == NULL);
    TEST_CHECK(q.tail == NULL);
    TEST_CHECK(q.size == 0);
}

#define MYTEST(X) { #X, X }

TEST_LIST = {
    MYTEST(test_DHQueue_init),
    { NULL, NULL }
};
