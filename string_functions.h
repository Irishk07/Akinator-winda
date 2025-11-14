#ifndef STRING_FUNCTIONS_H_
#define STRING_FUNCTIONS_H_

#include <stdio.h>

#include "stack.h"


const int SIZE_OF_BUF = 128;


void *my_recalloc(void *ptr, size_t new_size, size_t old_size);

void init_with_poisons(stack_type_t* ptr, size_t size);

ssize_t my_getline(char ** const lineptr, size_t * n, FILE * stream);


#endif //STRING_FUNCTIONS_H_