#include <assert.h>
#include <stdio.h>

#include "canary.h"

#include "stack.h"


void SettingCanariesToBegin(stack_type_t* ptr) {
    assert(ptr);
    
    *ptr = (stack_type_t)CANARY;
}

void SettingCanariesToEnd(stack_type_t* ptr, size_t capacity) {
    assert(ptr);
    assert(capacity > 0);
    assert(capacity < MAX_CAPACITY);

    *(ptr + capacity + 1) = (stack_type_t)CANARY;
}