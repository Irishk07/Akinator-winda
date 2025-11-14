#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>

#define TREE_CHECK_AND_RETURN_ERRORS(error, ...)                          \
        {                                                                 \
            Tree_status now_error = (error);                              \
            if (now_error != SUCCESS) {                                   \
                fprintf(stderr, "Error is: %d, %d\n", (error), __LINE__); \
                __VA_ARGS__;                                              \
                return now_error;                                         \
            }                                                             \
        }

#define TREE_STACK_CHECK_AND_RETURN_ERRORS(error, ...)                  \
        {                                                               \
            type_error_t now_error = (error);                           \
            if (now_error != STACK_SUCCESS) {                           \
                __VA_ARGS__;                                            \
                return STACK_ERROR;                                     \
            }                                                           \
        }
        

typedef char* type_t;
typedef const char* const_type_t;


struct Tree_node {
    type_t info;
    Tree_node* left_node;
    Tree_node* right_node;
    Tree_node* parent;
};

struct Dump_information {
    FILE* dump_file;
    const char* directory;
    int num_dump = 0;
};

struct Tree {
    Tree_node* root;
    size_t size;
    Dump_information dump_info;
};

struct Akinator {
    Tree tree;
    char* begin_buffer;
    char* end_buffer;
    size_t size_buffer;
    char* buffer_with_tree;
};


enum Tree_status {
    SUCCESS                  = 0,
    MEMORY_ERROR             = 1,
    OPEN_ERROR               = 2,
    EXECUTION_FAILED         = 3,
    CLOSE_ERROR              = 4,
    WRONG_SITUATION          = 5,
    READ_ERROR               = 6,
    NULL_POINTER_ON_TREE     = 7,
    WRONG_SIZE               = 8,
    NULL_POINTER_ON_NODE     = 9,
    PARENT_AND_CHILD_UNEQUAL = 10,
    WRONG_ROOT               = 11,
    WRONG_NODE               = 12,
    CHARACTER_NOT_FIND       = 13,
    SYNTAX_ERROR             = 14,
    BUFFER_OVERFLOW          = 15,
    FSEEK_ERROR              = 16,
    END_GAME                 = 17,
    STACK_ERROR              = 18,
    NULL_POINTER_ON_LEAF     = 19
};


#endif // COMMON_H_