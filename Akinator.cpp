#define TX_USE_SPEAK
#include "TXLib.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Akinator.h"

#include "colors.h"
#include "common.h"
#include "onegin.h"
#include "tree.h"
#include "stack.h"


Tree_status AkinatorCtor(Akinator* akinator, const char *dump_filename, const char *directory) {
    assert(akinator);
    assert(dump_filename);
    assert(directory);

    akinator->tree = {};
    TREE_CHECK_AND_RETURN_ERRORS(TreeCtor(&akinator->tree, dump_filename, directory));
    // TREE_CHECK_AND_RETURN_ERRORS(CreateRoot(&akinator->tree, "Bla-bla-bla"));

    akinator->begin_buffer = NULL;
    akinator->end_buffer   = NULL;
    akinator->size_buffer  = 0;

    return SUCCESS;
}

Tree_status StartAkinator(Akinator* akinator) {
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    color_printf(COLOR_PURPLE, " - Hello, friend!\n");

    color_printf(COLOR_PURPLE, " - Are you ready to start game?\n");

    while(true) {
        type_answer answer = GetAnswerYesNo();

        if (answer == YES) {
            color_printf(COLOR_PURPLE, " - Let's gooooooo\n");

            Tree_status status = ChooseOption(akinator);
            if (status == END_GAME) {
                color_printf(COLOR_PURPLE, " - Very sad... Bye-bye\n");
                break;
            }
            TREE_CHECK_AND_RETURN_ERRORS(status);

            color_printf(COLOR_PURPLE, " - So, our game is over. Do you want to continue?\n");
        }

        else if (answer == NO) {
            color_printf(COLOR_PURPLE, " - Very sad... Bye-bye\n");
            break;
        }

        else if (answer == WRONG_ANSWER) {
            color_printf(COLOR_RED, " - You try to do it 5 times... What's wrong with you?\n");
            break;
        }

        else if (answer == ERROR_ANSWER)
            TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR);
    }
    
    return SUCCESS;
}

type_answer GetAnswerYesNo() {
    type_answer result = WRONG_ANSWER;

    int cnt_attempts = CNT_ATTEMPTS;

    while (cnt_attempts-- > 0 && result == WRONG_ANSWER) {
        color_printf(COLOR_BLUE, " - Enter <Yes> or <No>\n");

        char* answer = ReadAnswer();

        if (strcmp(answer, "Yes") == 0) {
            result = YES;
        }
    
        if (strcmp(answer, "No") == 0) {
            result = NO;
        }

        free(answer);
    }

    return result;
}

#define DO_CASE(action) \
        action;         \
        break;

Tree_status ChooseOption(Akinator* akinator) {
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    color_printf(COLOR_YELLOW, " - Please, chose option:\n");
    color_printf(COLOR_YELLOW, "   [1] Guess a character\n");
    color_printf(COLOR_YELLOW, "   [2] Printf path to the character\n");
    color_printf(COLOR_YELLOW, "   [3] Compare two characters\n");
    color_printf(COLOR_YELLOW, "   [4] Save tree to file\n");
    color_printf(COLOR_YELLOW, "   [5] Upload tree from file\n");
    color_printf(COLOR_YELLOW, "   [6] Finish game\n");

    type_options option = ReadSelectedOption();

    switch (option) {
        case PLAY_AKINATOR:      DO_CASE(TREE_CHECK_AND_RETURN_ERRORS(PlayAkinator(akinator, akinator->tree.root)));
        case PATH_CHARACTER:     DO_CASE(TREE_CHECK_AND_RETURN_ERRORS(PathToCharacter(akinator)));
        case COMPARE_CHARACTERS: DO_CASE(TREE_CHECK_AND_RETURN_ERRORS(CompareTwoCharacters(akinator)));
        case SAVE_TREE:          DO_CASE(TREE_CHECK_AND_RETURN_ERRORS(SaveTree(akinator)));
        case UPLOAD_TREE:        DO_CASE(TREE_CHECK_AND_RETURN_ERRORS(UploadTree(akinator)));
        case END_AKINATOR:       DO_CASE(return END_GAME);
        case WRONG_OPTION:       DO_CASE(TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR));
        default:                 DO_CASE(TREE_CHECK_AND_RETURN_ERRORS(WRONG_SITUATION));
    }

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    return SUCCESS;
}

#undef DO_CASE

type_options ReadSelectedOption() {
    type_options option = WRONG_OPTION;
    int cur_option = WRONG_ANSWER;

    int cnt_attempts = CNT_ATTEMPTS;
    while (cnt_attempts-- > 0) {
        color_printf(COLOR_YELLOW, " - Enter 1, 2, 3, 4, 5 or 6\n");

        if (scanf("%d%*c", &cur_option) != 1) {
            scanf("%*[^\n]%*c");
        }
        option = (type_options)cur_option;
        
        if (option < WRONG_OPTION)
            return option;
    }

    return option;
}

Tree_status PlayAkinator(Akinator* akinator, Tree_node* cur_node) {
    assert(cur_node);
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree));

    while (cur_node->left_node != NULL && cur_node->right_node != NULL) {
        AskQuestion(cur_node);

        type_answer answer = GetAnswerYesNo();
        if (answer == ERROR_ANSWER)
            TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR);

        if (answer == YES)
            cur_node = (cur_node)->left_node;
        else
            cur_node = (cur_node)->right_node;
    }

    if ((cur_node)->left_node == NULL && (cur_node)->right_node == NULL) {
        type_answer answer = GiveAndCheckMyAnswer(cur_node);
        if (answer == ERROR_ANSWER)
            TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR);

        if (answer == YES) {
            color_printf(COLOR_GREEN,  " - I guessed right!\n");
            color_printf(COLOR_PURPLE, " - It was a wonderful game!\n");
        }
        else {
            color_printf(COLOR_PURPLE, " - Oh no:(\n");
            TREE_CHECK_AND_RETURN_ERRORS(AskAndAddRightAnswer(akinator, cur_node));
        }
    }
    else {
        TREE_CHECK_AND_RETURN_ERRORS(WRONG_SITUATION);
    }

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree));

    return SUCCESS;
}

void AskQuestion(Tree_node* cur_node) {
    assert(cur_node);

    color_printf(COLOR_GREEN, " - %s?\n", cur_node->info);
}

type_answer GiveAndCheckMyAnswer(Tree_node* cur_node) {
    assert(cur_node);

    color_printf(COLOR_GREEN, " - So, my answer: %s\n", cur_node->info);

    color_printf(COLOR_PURPLE, " - Is it right?\n");

    return GetAnswerYesNo();
}

Tree_status AskAndAddRightAnswer(Akinator* akinator, Tree_node* cur_node) {
    assert(cur_node);
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    color_printf(COLOR_CYAN, " - What is the right answer?\n");

    char* right_answer = ReadAnswer();

    if (right_answer == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR);

    color_printf(COLOR_CYAN, " - Ask, please, how is %s different from %s. It...?\n", right_answer, cur_node->info);

    char* difference = ReadAnswer();

    if (difference == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR,    free(right_answer));

    TREE_CHECK_AND_RETURN_ERRORS(InsertTwoLeaves(&akinator->tree, &cur_node, right_answer, difference),     free(right_answer);
                                                                                                            free(difference););

    color_printf(COLOR_GREEN, " - Thanks! It's very interesting. I remembered it\n");

    free(right_answer);
    free(difference);

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    return SUCCESS;
}

Tree_status PathToCharacter(Akinator* akinator) {
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree));

    color_printf(COLOR_PURPLE, " - Enter name of the character\n");

    stack_t stack = {};
    char* character = NULL;
    TREE_CHECK_AND_RETURN_ERRORS(ReadAndDefinitionCharacter(akinator, &character, &stack));

    color_printf(COLOR_GREEN, " - Path to your character:\n");

    size_t cnt_connecting_words = sizeof(connecting_words) / sizeof(connecting_words[0]);

    color_printf(COLOR_GREEN, " --- %s: ", character);
    
    Tree_node* cur_node = akinator->tree.root;

    for (size_t i = 0; i < stack.size - 1; ++i) {
        size_t num_of_word = (size_t)rand() % cnt_connecting_words;

        PrintCurNode(&stack, i, cur_node);

        color_printf(COLOR_GREEN, "%s ", connecting_words[num_of_word]);

        TREE_CHECK_AND_RETURN_ERRORS(MoveToNextNode(&stack, i, &cur_node));
    }
    // printf last sign, without common
    if (stack.data[stack.size - 1] == LEFT_NODE)
        color_printf(COLOR_GREEN, "%s\n", cur_node->info);
    else
        color_printf(COLOR_GREEN, "Not %s\n", cur_node->info);

    TREE_STACK_CHECK_AND_RETURN_ERRORS(StackDtor(&stack));

    free(character);

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree));

    return SUCCESS;
}

Tree_status ReadAndDefinitionCharacter(Akinator* akinator, char** character, stack_t* stack) {
    assert(character);
    assert(stack);
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree));

    *character = ReadAnswer();
    if (character == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR);

    TREE_CHECK_AND_RETURN_ERRORS(DefinitionOfCharacter(akinator, stack, character));

    return SUCCESS;
}

Tree_status DefinitionOfCharacter(Akinator* akinator, stack_t* stack, char** character) {
    assert(stack);
    assert(character);
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree));

    TREE_STACK_CHECK_AND_RETURN_ERRORS(StackCtor(stack, DEFAULT_START_CAPACITY));

    if (FindCharacter(stack, akinator->tree.root, *character) == NULL)
        return CHARACTER_NOT_FIND;

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    return SUCCESS;
}

Tree_node* FindCharacter(stack_t* stack, Tree_node* tree_node, type_t character) {
    assert(stack);
    assert(tree_node);

    if (tree_node == NULL)
        return NULL;

    if (strcmp(tree_node->info, character) == 0)
        return tree_node;

    Tree_node* result = NULL;
    StackPush(stack, LEFT_NODE);
    if (tree_node->left_node)
        result = FindCharacter(stack, tree_node->left_node, character);

    if (result != NULL)
        return result;
    StackPop(stack, NULL);

    StackPush(stack, RIGHT_NODE);
    if (tree_node->right_node)
        result = FindCharacter(stack, tree_node->right_node, character);

    if (result != NULL)
        return result;
    StackPop(stack, NULL);

    return NULL;
}

Tree_status CompareTwoCharacters(Akinator* akinator) {
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree));

    color_printf(COLOR_PURPLE, " - Enter name of the first character\n");
    char* first_character = NULL;
    stack_t first_stack = {};
    TREE_CHECK_AND_RETURN_ERRORS(ReadAndDefinitionCharacter(akinator, &first_character, &first_stack));

    color_printf(COLOR_PURPLE, " - Enter name of the second character\n");
    char* second_character = NULL;
    stack_t second_stack = {};
    TREE_CHECK_AND_RETURN_ERRORS(ReadAndDefinitionCharacter(akinator, &second_character, &second_stack));

    color_printf(COLOR_GREEN, " - Now you can see common signs of %s and %s: ", first_character, second_character);

    size_t both_size = MinSize_t(first_stack.size, second_stack.size);
    size_t cur_size = 0;

    Tree_node* cur_node = akinator->tree.root;
    for (; cur_size < both_size; ++cur_size) {
        if (first_stack.data[cur_size] != second_stack.data[cur_size]) {
            break;
        }

        PrintCurNode(&first_stack, cur_size, cur_node);

        TREE_CHECK_AND_RETURN_ERRORS(MoveToNextNode(&first_stack, cur_size, &cur_node));
    }
    printf("\n");

    Tree_node* cur_node_first = cur_node;
    Tree_node* cur_node_second = cur_node;

    color_printf(COLOR_GREEN, " - Now you can see diffrent signs:\n");

    color_printf(COLOR_GREEN, " - First character %s has such signs: ", first_character);
    TREE_CHECK_AND_RETURN_ERRORS(PrintDifferentSigns(cur_node_first, &first_stack, cur_size));

    color_printf(COLOR_GREEN, " - Second character %s has such signs: ", second_character);
    TREE_CHECK_AND_RETURN_ERRORS(PrintDifferentSigns(cur_node_second, &second_stack, cur_size));

    TREE_STACK_CHECK_AND_RETURN_ERRORS(StackDtor(&first_stack));
    TREE_STACK_CHECK_AND_RETURN_ERRORS(StackDtor(&second_stack));

    free(first_character);
    free(second_character);

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree));

    return SUCCESS;
}

size_t MinSize_t(size_t first_value, size_t second_value) {
    return (first_value < second_value) ? first_value : second_value;
}

void PrintCurNode(stack_t* stack, size_t index, Tree_node* cur_node) {
    assert(stack);
    assert(cur_node);

    if (stack->data[index] == LEFT_NODE)
        color_printf(COLOR_GREEN, "%s, ", cur_node->info);
    else if (stack->data[index] == RIGHT_NODE)
        color_printf(COLOR_GREEN, "Not %s, ", cur_node->info);
}

Tree_status MoveToNextNode(stack_t* stack, size_t index, Tree_node** cur_node) {
    assert(stack);
    assert(cur_node);
    assert(*cur_node);

    if (stack->data[index] == LEFT_NODE) {
        if ((*cur_node)->left_node == NULL)
            TREE_CHECK_AND_RETURN_ERRORS(NULL_POINTER_ON_LEAF);

        *cur_node = (*cur_node)->left_node;
    }

    else if (stack->data[index] == RIGHT_NODE) {
        if ((*cur_node)->right_node == NULL)
            TREE_CHECK_AND_RETURN_ERRORS(NULL_POINTER_ON_LEAF);

        *cur_node = (*cur_node)->right_node;
    }

    return SUCCESS;
}

Tree_status PrintDifferentSigns(Tree_node* cur_node, stack_t* stack, size_t cur_size) {
    assert(cur_node);
    assert(stack);

    for (size_t i = cur_size; i < stack->size - 1; ++i) {
        PrintCurNode(stack, i, cur_node);

        TREE_CHECK_AND_RETURN_ERRORS(MoveToNextNode(stack, i, &cur_node));
    }
    // printf last sign, without common
    if (stack->data[stack->size - 1] == LEFT_NODE)
        color_printf(COLOR_GREEN, "%s\n", cur_node->info);
    else
        color_printf(COLOR_GREEN, "Not %s\n", cur_node->info);

    return SUCCESS;
}

Tree_status SaveTree(Akinator* akinator) {
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    color_printf(COLOR_PURPLE, " - Please, enter the name of the file where the tree will be saved\n");

    char* file_name = ReadAnswer();

    if (file_name == NULL)
        color_printf(COLOR_RED, " - Sorry, I couldn't do it, I have some problems. Try later please\n");
    else
        TREE_CHECK_AND_RETURN_ERRORS(CreateTreeFile(&akinator->tree, file_name));

    color_printf(COLOR_GREEN, " - All right, tree is saved in your file %s\n", file_name);

    free(file_name);

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    return SUCCESS;
}

Tree_status UploadTree(Akinator* akinator) {
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    color_printf(COLOR_PURPLE, " - Please, enter the name of the file from which the tree will be uploaded\n");

    char* file_name = ReadAnswer();

    if (file_name == NULL)
        color_printf(COLOR_RED, " - Sorry, I couldn't do it, I have some problems. Try later please\n");
    else
        TREE_CHECK_AND_RETURN_ERRORS(ReadTree(akinator, file_name));

    color_printf(COLOR_GREEN, " - All right, tree is uploaded from your file %s\n", file_name);

    ON_DEBUG(TREE_CHECK_AND_RETURN_ERRORS(TreeHTMLDump(&akinator->tree, akinator->tree.root, DUMP_INFO, NOT_ERROR_DUMP)));

    free(file_name);

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree));

    return SUCCESS;
}

Tree_status ReadTree(Akinator* akinator, const char* file_with_tree) {
    assert(file_with_tree);
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree));

    TREE_CHECK_AND_RETURN_ERRORS(ReadOnegin(akinator, file_with_tree));

    akinator->begin_buffer = akinator->buffer_with_tree;
    akinator->end_buffer   = akinator->begin_buffer + akinator->size_buffer;

    akinator->tree.root = NULL;
    TREE_CHECK_AND_RETURN_ERRORS(ReadNode(akinator, &akinator->tree.root, &akinator->buffer_with_tree, NULL));

    ON_DEBUG(TREE_CHECK_AND_RETURN_ERRORS(TreeHTMLDump(&akinator->tree, akinator->tree.root, DUMP_INFO, NOT_ERROR_DUMP)));

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree));

    return SUCCESS;
}

#define DUMP_CURRENT_SITUATION(tree, tree_node, current_pos)                        \
    TreeHTMLDump(&tree, tree_node, DUMP_INFO, NOT_ERROR_DUMP);                      \
    fprintf(tree.dump_info.dump_file, "Current situation: \":%s\"\n", *current_pos);

Tree_status ReadNode(Akinator* akinator, Tree_node** tree_node, char** current_pos, Tree_node* parent) {    
    assert(akinator);
    assert(tree_node);
    assert(current_pos);
    assert(*current_pos);

    SkipSpaces(current_pos);

    if (*current_pos > akinator->end_buffer)
        TREE_CHECK_AND_RETURN_ERRORS(BUFFER_OVERFLOW);

    if (**current_pos == '(') {
        NodeCtor(&akinator->tree, tree_node, parent);
        ON_DEBUG(DUMP_CURRENT_SITUATION(akinator->tree, *tree_node, current_pos));

        (*current_pos)++;
        ON_DEBUG(DUMP_CURRENT_SITUATION(akinator->tree, *tree_node, current_pos));

        int read_bytes = 0;
        sscanf(*current_pos, "\"%*[^\"]\"%n", &read_bytes);
        *(*current_pos + read_bytes - 1) = '\0'; // close " -> '\0'

        FillNodeInfo(*tree_node, *current_pos + 1); // ++ because skip "
        ON_DEBUG(DUMP_CURRENT_SITUATION(akinator->tree, *tree_node, current_pos));

        *current_pos += read_bytes;
        ON_DEBUG(DUMP_CURRENT_SITUATION(akinator->tree, *tree_node, current_pos));

        TREE_CHECK_AND_RETURN_ERRORS(ReadNode(akinator, &(*tree_node)->left_node, current_pos, *tree_node));

        TREE_CHECK_AND_RETURN_ERRORS(ReadNode(akinator, &(*tree_node)->right_node, current_pos, *tree_node));

        (*current_pos)++; // ++ because skip ')'
        ON_DEBUG(DUMP_CURRENT_SITUATION(akinator->tree, *tree_node, current_pos));

        SkipSpaces(current_pos);
    }

    else if (strncmp(*current_pos, "nil", LEN_NIL) == 0) {
        *current_pos += LEN_NIL;
        ON_DEBUG(DUMP_CURRENT_SITUATION(akinator->tree, *tree_node, current_pos));

        *tree_node = NULL;
    }

    else {
        TREE_CHECK_AND_RETURN_ERRORS(SYNTAX_ERROR);
    }

    return SUCCESS;
}

#undef DUMP_CURRENT_SITUATION

Tree_status AkinatorDtor(Akinator* akinator) {
    Tree_status status = TreeVerify(&akinator->tree);

    free(akinator->begin_buffer);

    AkinatorNodeDtor(akinator, akinator->tree.root);
    akinator->tree.root = NULL;

    if (fclose(akinator->tree.dump_info.dump_file) == EOF) {
        status = CLOSE_ERROR;
        perror("Error is: ");
    }

    akinator->tree.dump_info.dump_file = NULL;

    akinator->tree.size = 0;

    return status;
}

void AkinatorNodeDtor(Akinator* akinator, Tree_node* tree_node) {
    if (tree_node == NULL)
        return;

    if (tree_node->info < akinator->begin_buffer || tree_node->info > akinator->end_buffer)
        free(tree_node->info);
    tree_node->info = NULL;

    AkinatorNodeDtor(akinator, tree_node->left_node);

    AkinatorNodeDtor(akinator, tree_node->right_node);

    free(tree_node);
}