#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

#include "common.h"
#include "debug.h"
#include "string_functions.h"


Tree_status TreeCtor(Tree* tree, const char* dump_filename, const char* directory) {
    assert(tree);
    assert(dump_filename);
    assert(directory);

    tree->size = 0;

    tree->dump_info.directory = directory;

    FILE* dump_file = fopen(dump_filename, "w");
    if (dump_file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR,    free(tree->root));

    tree->dump_info.dump_file = dump_file;

    return SUCCESS;
}

Tree_status NodeCtor(Tree* tree, Tree_node** new_node, Tree_node* parent) { // TODO assert
    assert(tree);
    assert(new_node);

    *new_node = (Tree_node*)calloc(1, sizeof(Tree_node));
    if (*new_node == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(MEMORY_ERROR);

    (*new_node)->left_node = NULL;
    (*new_node)->right_node = NULL;

    (*new_node)->parent = parent;

    tree->size++;

    return SUCCESS;
}

Tree_status CreateRoot(Tree* tree, const_type_t info) {
    assert(tree);
    assert(info);

    tree->root = NULL;
    TREE_CHECK_AND_RETURN_ERRORS(NodeCtor(tree, &tree->root, NULL));
    FillNodeInfo(tree->root, info);

    tree->root->left_node  = NULL;
    tree->root->right_node = NULL;
    tree->root->parent     = NULL;

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(tree)); 

    return SUCCESS;
}

Tree_status FillNodeInfo(Tree_node* tree_node, const_type_t info) {
    assert(tree_node);
    assert(info);

    if (tree_node->info != NULL)
        free(tree_node->info);

    tree_node->info = strdup(info);

    if (tree_node->info == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(MEMORY_ERROR);

            // another variant
            // type_t new_info = (type_t)calloc(strlen(question) + 2, sizeof(char));
            // if (new_info == NULL) 
            //     TREE_CHECK_AND_RETURN_ERRORS(MEMORY_ERROR);
        
            // if ((*node)->info != NULL)
            //     free((*node)->info);
        
            // strcpy(new_info, question);
            // (*node)->info = new_info;

    return SUCCESS;
}

Tree_status TreeVerify(Tree* tree) {
    if (tree == NULL)
        TREE_DUMP_AND_RETURN_ERRORS(NULL_POINTER_ON_TREE);

    if (tree->size != TreeSize(tree->root))
        TREE_DUMP_AND_RETURN_ERRORS(WRONG_SIZE);

    TREE_DUMP_AND_RETURN_ERRORS(AllNodesVerify(tree, tree->root));

    return SUCCESS;
}

Tree_status AllNodesVerify(Tree* tree, Tree_node* tree_node) {
    if (tree_node == NULL)
        return SUCCESS;

    TREE_CHECK_AND_RETURN_ERRORS(OneNodeVerify(tree, tree_node));

    TREE_CHECK_AND_RETURN_ERRORS(AllNodesVerify(tree, tree_node->left_node));

    TREE_CHECK_AND_RETURN_ERRORS(AllNodesVerify(tree, tree_node->right_node));

    return SUCCESS;
}

Tree_status OneNodeVerify(Tree* tree, Tree_node* tree_node) {
    if (tree_node == NULL)            TREE_CHECK_AND_RETURN_ERRORS(NULL_POINTER_ON_NODE);

    if (tree_node == tree->root && 
        tree_node->parent != NULL)    TREE_CHECK_AND_RETURN_ERRORS(WRONG_ROOT);

    if (tree_node != tree->root &&
        tree_node->parent == NULL)    TREE_CHECK_AND_RETURN_ERRORS(WRONG_NODE);

    if (tree_node != tree->root) {
        if (tree_node->parent->left_node != tree_node &&
            tree_node->parent->right_node != tree_node) TREE_CHECK_AND_RETURN_ERRORS(PARENT_AND_CHILD_UNEQUAL);
    }
    return SUCCESS;
}

size_t TreeSize(Tree_node* tree_node) {
    if (tree_node == NULL) {
        return 0;
    }
    
    return 1 + TreeSize(tree_node->left_node) + TreeSize(tree_node->right_node);
}

Tree_status InsertTwoLeaves(Tree* tree, Tree_node** node, const_type_t answer, const_type_t question) { // const_type_t
    assert(node);
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(tree));

    Tree_node* left_new_node = NULL;
    TREE_CHECK_AND_RETURN_ERRORS(NodeCtor(tree, &left_new_node, *node));
    TREE_CHECK_AND_RETURN_ERRORS(FillNodeInfo(left_new_node, answer));

    Tree_node* right_new_node = NULL;
    TREE_CHECK_AND_RETURN_ERRORS(NodeCtor(tree, &right_new_node, *node));
    TREE_CHECK_AND_RETURN_ERRORS(FillNodeInfo(right_new_node, (*node)->info));

    (*node)->left_node  = left_new_node;
    (*node)->right_node = right_new_node;

    TREE_CHECK_AND_RETURN_ERRORS(FillNodeInfo(*node, question));

    ON_DEBUG(TREE_CHECK_AND_RETURN_ERRORS(TreeHTMLDump(tree, tree->root, DUMP_INFO, NOT_ERROR_DUMP)));

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(tree));

    return SUCCESS;
}

char* ReadAnswer() {
    char* answer = NULL;
    size_t size_buf = 0;

    if (my_getline(&answer, &size_buf, stdin) == -1)
        return NULL;

    return answer;
}

Tree_status CreateTreeFile(Tree* tree, const char* name_file_with_tree) {
    assert(name_file_with_tree);
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(tree));

    FILE* file = fopen(name_file_with_tree, "w");
    if (file == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    PrintTreeToFile(tree->root, file);

    if (fclose(file) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR);

    return SUCCESS;
}

void PrintTreeToFile(Tree_node* tree_node, FILE* stream) {
    if (tree_node == NULL) {
        fprintf(stream, "nil");
        return;
    }
    
    fprintf(stream, "(\"%s\" ", tree_node->info);

    PrintTreeToFile(tree_node->left_node, stream);
    fprintf(stream, " ");

    PrintTreeToFile(tree_node->right_node, stream);
    fprintf(stream, ")");
}

void SkipSpaces(char** buffer) {
    assert(buffer);
    assert(*buffer);

    while (isspace(**buffer) && (**buffer) != '\0')
        (*buffer)++;
}

Tree_status TreeDtor(Tree* tree) {
    Tree_status status = TreeVerify(tree);

    NodeDtor(tree->root);
    tree->root = NULL;

    if (fclose(tree->dump_info.dump_file) == EOF) {
        status = CLOSE_ERROR;
        perror("Error is: ");
    }

    tree->dump_info.dump_file = NULL;

    tree->size = 0;

    return status;
}

void NodeDtor(Tree_node* tree_node) {
    if (tree_node == NULL)
        return;

    free(tree_node->info);
    tree_node->info = NULL;

    NodeDtor(tree_node->left_node);

    NodeDtor(tree_node->right_node);

    free(tree_node);
}

Tree_status TreeHTMLDump(Tree* tree, Tree_node* tree_node, int line, const char* file, Type_dump type_dump, Tree_status tree_status) {
    fprintf(tree->dump_info.dump_file, "<pre> <font size = \"8\">\n");

    if (type_dump == USUAL_DUMP)
        fprintf(tree->dump_info.dump_file, "<h3> DUMP <font color = green> Tree </font> </h3>\n");
    
    if (type_dump == ERROR_DUMP) {
        fprintf(tree->dump_info.dump_file, "<h3> ERROR_ANSWER DUMP <font color = red> Tree </font> </h3>\n");

        PrintErrors(tree_status, tree->dump_info.dump_file);

        if (tree_status == NULL_POINTER_ON_TREE ||
            tree_status == NULL_POINTER_ON_NODE)
            return tree_status;
    }

    fprintf(tree->dump_info.dump_file, "Tree {%s: %d}\n", file, line);

    fprintf(tree->dump_info.dump_file, "Size: %zu\n", tree->size);

    fprintf(tree->dump_info.dump_file, "Root: %p\n", tree->root);

    TREE_CHECK_AND_RETURN_ERRORS(GenerateGraph(tree, tree_node));

    char command[MAX_LEN_NAME] = {};
    snprintf(command, MAX_LEN_NAME, "dot %s/graphes/graph%d.txt -T png -o %s/images/image%d.png", 
                                     tree->dump_info.directory, tree->dump_info.num_dump, tree->dump_info.directory, tree->dump_info.num_dump);
    
    fprintf(stderr, "start\n");
    if (system((const char*)command) != 0)
        TREE_CHECK_AND_RETURN_ERRORS(EXECUTION_FAILED,      fprintf(tree->dump_info.dump_file, "Error with create image:(\n"));
    fprintf(stderr, "finish\n");

    fprintf(tree->dump_info.dump_file, "\n");
    fprintf(tree->dump_info.dump_file, "<img src = %s/images/image%d.png width = 700px>", tree->dump_info.directory, tree->dump_info.num_dump);

    fprintf(tree->dump_info.dump_file, "\n\n");

    tree->dump_info.num_dump++;

    return SUCCESS;
}

Tree_status GenerateGraph(Tree* tree, Tree_node* tree_node) {
    char filename_graph[MAX_LEN_NAME] = {};
    snprintf(filename_graph, MAX_LEN_NAME, "%s/graphes/graph%d.txt", tree->dump_info.directory, tree->dump_info.num_dump);

    FILE* graph = fopen(filename_graph, "w");
    if (graph == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(OPEN_ERROR);

    fprintf(graph, "digraph {\n");
    fprintf(graph, "    node [shape = Mrecord, style = filled, fillcolor = \"#99FF99\"];\n");

    if (tree_node != NULL) {
        PrintNodeToDot(tree, graph, tree_node);
    } 
    else {
        fprintf(graph, "There will be no tree\n");
    }

    fprintf(graph, "}\n");

    if (fclose(graph) == EOF)
        TREE_CHECK_AND_RETURN_ERRORS(CLOSE_ERROR,    perror("Error is:"));

    return SUCCESS;
}

void PrintNodeToDot(Tree* tree, FILE *file, Tree_node* tree_node) {
    assert(file);
    assert(tree_node);

    if (tree_node == tree->root)
        fprintf(file, "    node_%p [fillcolor = \"#de86f5ff\", label=<{%p | %s | parent = %p | left = %p | right = %p}>];\n", 
                      (void *)tree_node, tree_node, tree_node->info, tree_node->parent, tree_node->left_node, tree_node->right_node);

    else {
        fprintf(file, "    node_%p [label=\"{%p | %s | parent = %p | left = %p | right = %p}\"];\n", 
                      (void *)tree_node, tree_node, tree_node->info, tree_node->parent, tree_node->left_node, tree_node->right_node);
    }

    if (tree_node->left_node) {
        fprintf(file, "    node_%p -> node_%p [label = \"Yes\", color = \"#FF6600\"];\n ", (void *)tree_node, (void *)tree_node->left_node);
        PrintNodeToDot(tree, file, tree_node->left_node);
    }

    if (tree_node->right_node) {
        fprintf(file, "    node_%p -> node_%p [label = \"No\", color = \"#0000CC\"];\n", (void *)tree_node, (void *)tree_node->right_node);
        PrintNodeToDot(tree, file, tree_node->right_node);
    }
}

void PrintErrors(int error, FILE* stream) {
    if (error == SUCCESS                  ) fprintf(stream, "ALL_RIGHT\n");
    if (error == NULL_POINTER_ON_TREE     ) fprintf(stream, "Null pointer on tree\n");
    if (error == NULL_POINTER_ON_NODE     ) fprintf(stream, "Null pointer on node\n");
    if (error == MEMORY_ERROR             ) fprintf(stream, "Not enough memory\n");
    if (error == OPEN_ERROR               ) fprintf(stream, "Open error\n");
    if (error == CLOSE_ERROR              ) fprintf(stream, "Close error\n");
    if (error == EXECUTION_FAILED         ) fprintf(stream, "Execution failed\n");
    if (error == WRONG_SITUATION          ) fprintf(stream, "Left node is NULL, but right node not OR right node is NULL, but left node not\n");
    if (error == READ_ERROR               ) fprintf(stream, "Scanf can't read user's answers\n");
    if (error == WRONG_SIZE               ) fprintf(stream, "Size of tree is wrong\n");
    if (error == PARENT_AND_CHILD_UNEQUAL ) fprintf(stream, "Parent's chils is unequal with current node\n");
    if (error == WRONG_ROOT               ) fprintf(stream, "Node is root, but its parent isn't NULL\n");
    if (error == WRONG_NODE               ) fprintf(stream, "Node isn't root, but its parent is NULL\n");
}