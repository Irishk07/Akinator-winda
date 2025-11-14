#ifndef CANARY_H_
#define CANARY_H_

#include <stdio.h>

#include "debug.h"
#include "stack.h"

#ifdef CANARIES
const size_t CNT_CANARIES = 2;
#else
const size_t CNT_CANARIES = 0;
#endif //CANARIES


const int CANARY = 0XEDA; // TODO: what if stack_type_t is not an integer type?


void SettingCanariesToBegin(stack_type_t* ptr);

void SettingCanariesToEnd(stack_type_t* ptr, size_t capacity);


#endif //CANARY_H_