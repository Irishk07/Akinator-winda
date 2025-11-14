#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "onegin.h"

#include "common.h"


long SizeOfText(FILE* file) {
    if (fseek(file, 0, SEEK_END) != 0) {
        return -1;
    }
    
    long size = ftell(file);

    if (fseek(file, 0, SEEK_SET) != 0) {
        return -1;
    }
    
    return size;
}

Tree_status ReadOnegin(Akinator* akinator, const char* name_file) {
    assert(akinator != NULL);    

    FILE *text = fopen(name_file, "r");
    if (text == NULL) {
        perror("Error is");
        return OPEN_ERROR;
    }

    int size = SizeOfText(text);
    if (size == -1) {
        TREE_CHECK_AND_RETURN_ERRORS(FSEEK_ERROR,   fclose(text));
    }
    akinator->size_buffer = (size_t)size;

    akinator->buffer_with_tree = (char*)calloc((akinator->size_buffer + 1), sizeof(char));
    if (akinator->buffer_with_tree == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(MEMORY_ERROR);

    fread((char *)akinator->buffer_with_tree, sizeof(char), akinator->size_buffer, text);
    if (ferror(text) != 0) {
        TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR,    free(akinator->buffer_with_tree));
    }

    return SUCCESS;
}