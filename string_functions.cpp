#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "string_functions.h"

#include "stack.h"


void* my_recalloc(void* ptr, size_t new_size, size_t old_size) {
    assert(new_size > 0);
    assert(new_size < MAX_CAPACITY);
    assert(old_size < MAX_CAPACITY);

    char* temp = (char*)realloc(ptr, new_size);

    if (temp == NULL) {
        return NULL;
    }

    if (new_size <= old_size) {
        return temp;
    }

    memset(temp + old_size, 0, new_size - old_size);

    return temp;
}

void init_with_poisons(stack_type_t* ptr, size_t size) { 
    assert(ptr);
    assert(size > 0);
    assert(size < MAX_CAPACITY);

    for (size_t i = 0; i < size; ++i) {
        *(ptr + i) = DEFAULT_POISON;
    }
}

ssize_t my_getline(char ** const lineptr, size_t* n, FILE * stream) {
    if (n == NULL || lineptr == NULL) {
        return -1;
    }
    
    if (*lineptr == NULL) {
        *n = 0;
    } 

    int ch = 0;
    size_t size = 0;

    char* current_pos = *lineptr;

    while ((ch = getc(stream)) != '\n' && ch != EOF) {
        if (*n < size + 1) {
            char* temp = (char*)my_recalloc(*lineptr, (*n == 0) ? SIZE_OF_BUF : 2 * (*n), *n);
            if (temp == NULL) {
                return -1;
            }

            *n = (*n == 0) ? SIZE_OF_BUF : 2 * (*n);

            current_pos = temp + (current_pos - *lineptr);
            *lineptr = temp;
        }
        *current_pos = (char)ch;
        (current_pos)++;

        size++;
    }

    *current_pos = '\0';

    if (ferror(stream) != 0) {
        return -1;
    }

    return (ssize_t)size;
}