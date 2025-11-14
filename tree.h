#ifndef TREE_H_
#define TREE_H_

#include <stdio.h>

#include "common.h"


#define TREE_DUMP_AND_RETURN_ERRORS(error, ...)                            \
        {                                                                        \
            Tree_status now_error = error;                                       \
            if (now_error != SUCCESS) {                                          \
                TreeHTMLDump(tree, tree->root, DUMP_INFO, ERROR_DUMP, now_error); \
                return now_error;                                                \
            }                                                                    \
        }

#define DUMP_INFO __LINE__, __FILE__
#define NOT_ERROR_DUMP USUAL_DUMP, SUCCESS


const int MAX_LEN_NAME = 100;
const int LEN_NIL      = 3;


enum Type_dump {
    USUAL_DUMP = 0,
    ERROR_DUMP = 1
};


Tree_status TreeCtor(Tree* tree, const char* dump_filename, const char* directory);

Tree_status NodeCtor(Tree* tree, Tree_node** new_node, Tree_node* parent);

Tree_status CreateRoot(Tree* tree, const_type_t info);

Tree_status FillNodeInfo(Tree_node* tree_node, const_type_t info);

Tree_status TreeVerify(Tree* tree);

Tree_status AllNodesVerify(Tree* tree, Tree_node* tree_node);

Tree_status OneNodeVerify(Tree* tree, Tree_node* tree_node);

size_t TreeSize(Tree_node* tree_node);

Tree_status InsertTwoLeaves(Tree* tree, Tree_node** node, const_type_t answer, const_type_t question);

char* ReadAnswer();

Tree_status CreateTreeFile(Tree* tree, const char* name_file_with_tree);

void PrintTreeToFile(Tree_node* tree_node, FILE* stream);

void SkipSpaces(char** buffer);

Tree_status TreeDtor(Tree* tree);

void NodeDtor(Tree_node* tree_node);

Tree_status TreeHTMLDump(Tree* tree, Tree_node* tree_node, int line, const char* file, Type_dump type_dump, Tree_status tree_status);

Tree_status GenerateGraph(Tree* tree, Tree_node* tree_node);

void PrintNodeToDot(Tree* tree, FILE *file, Tree_node* tree_node);

void PrintErrors(int error, FILE* stream);


#endif // TREE_H_