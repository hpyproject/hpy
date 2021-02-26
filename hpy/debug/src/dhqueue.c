#include "debug_internal.h"

void DHQueue_init(DHQueue *q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}
