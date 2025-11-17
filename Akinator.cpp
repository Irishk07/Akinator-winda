#define TX_USE_SPEAK
#define TX_COMPILED
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
#include "string_functions.h"

#define CONSOLE_POS                      \
        txSetConsoleCursorPos(350, 250); 

#define DEFAULT_SETTINGS_BEGIN(text_size)\
        txSetFillColor(TX_BLACK);        \
        txSetColor(TX_WHITE);            \
        txSelectFont("Arial", text_size);\
        CONSOLE_POS                      \
        txTextCursor(true);              \
        txClear();

#define DEFAULT_SETTINGS_END \
        txTextCursor(false); \
        txClear();


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

// system(start \min file)

Tree_status StartAkinator(Akinator* akinator) {
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    txCreateWindow(800, 600);
    txTextCursor(false);

    TXImage("downloads/akinator.bmp");

    txSpeak("<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xml:lang='EN'>" 
            "                     Now.... Akinator\n"
            "</speak>");

    txSpeak("\aHello friend!");
    TXVideo(5, 1, 1500);

    txSpeak("\aAre you ready to start game?");
    TXVideo(26, 2, 1500);

    while(true) {
        type_answer answer = GetAnswerYesNo();

        if (answer == YES) {
            txSpeak("\aLet's gooooooo");
            TXVideo(7, 10, 1000);

            Tree_status status = ChooseOption(akinator);
            if (status == END_GAME) {
                txSpeak("\aVery sad... Bye-bye");
                TXVideo(100, 3, 2000);
                break;
            }
            TREE_CHECK_AND_RETURN_ERRORS(status);

            txSpeak("\aSo, our game is over. Do you want to continue?");
            TXVideo(26, 2, 2000);
        }

        else if (answer == NO) {
            txSpeak("\aVery sad... Bye-bye");
            TXVideo(100, 3, 2000);
            break;
        }

        else if (answer == WRONG_ANSWER) {
            txSpeak("\aYou try to do it 5 times... What's wrong with you?");
            TXVideo(100, 3, 2000);
            break;
        }

        else if (answer == ERROR_ANSWER)
            TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR);
    }
    
    return SUCCESS;
}

void TXImage(const char* filename) {
    HDC image = txLoadImage(filename);

    txBitBlt(txDC(), 0, 0, 0, 0, image);
}

void TXVideo(int cnt_shots, int number_video, int sleep) {    
    for (int i = 0; i < cnt_shots; i++) {
        char filename[100];
        snprintf(filename, 100, "downloads/images%d/image%d.bmp", number_video, i + 1);

        HDC image = txLoadImage(filename);

        txBitBlt(txDC(), 0, 0, 0, 0, image);

        txDeleteDC(image);

        if (txGetAsyncKeyState(VK_ESCAPE))
            break;

        txSleep(15);
    }

    txSleep(sleep);
}

type_answer GetAnswerYesNo() {
    type_answer result = WRONG_ANSWER;

    int cnt_attempts = CNT_ATTEMPTS;

    DEFAULT_SETTINGS_BEGIN(80);


    while (cnt_attempts-- > 0 && result == WRONG_ANSWER) {
        CONSOLE_POS;
        txDrawText(100, 150, 700, 500, "Enter <Yes> or <No>\n"
                                        "_________________\n");

        char* answer = ReadAnswer();

        if (strcmp(answer, "Yes") == 0) {
            result = YES;
        }
    
        if (strcmp(answer, "No") == 0) {
            result = NO;
        }

        free(answer);

        txClearConsole();
    }

    DEFAULT_SETTINGS_END;

    return result;
}

char* ReadAnswer() {
    char* answer = NULL;
    size_t size_buf = 0;

    DEFAULT_SETTINGS_BEGIN(80);
    txDrawText(100, 150, 700, 500, "Enter your answer\n"
                                    "________________\n");

    if (my_getline(&answer, &size_buf, stdin) == -1)
        return NULL;

    txClearConsole();
    DEFAULT_SETTINGS_END;

    return answer;
}

#define DO_CASE(action) \
        action;         \
        break;

Tree_status ChooseOption(Akinator* akinator) {
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    DEFAULT_SETTINGS_BEGIN(40);
    txTextCursor(false);

    txDrawText(100, 70, 700, 550, "Available options:\n\n" 
                                   "[1] Guess a character\n"
                                   "[2] Printf path to the character\n"
                                   "[3] Compare two characters\n"
                                   "[4] Save tree to file\n"
                                   "[5] Upload tree from file\n"
                                   "[6] Finish game\n"
                                   "\n\n"
                                   "Press any button\n");
    txSpeak("\aPlease, chose option");
    getchar();
    txClearConsole();

    DEFAULT_SETTINGS_END;

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

    DEFAULT_SETTINGS_BEGIN(80);

    int cnt_attempts = CNT_ATTEMPTS;
    while (cnt_attempts-- > 0) {
        CONSOLE_POS;
        txDrawText(100, 150, 700, 500, "Enter 1, 2, 3, 4, 5 or 6\n"
                                        "_________________\n");

        if (scanf("%d%*c", &cur_option) != 1) {
            scanf("%*[^\n]%*c");
        }
        option = (type_options)cur_option;
        
        if (option < WRONG_OPTION)
            break;

        txClearConsole();
    }

    txClearConsole();
    DEFAULT_SETTINGS_END;

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
            txSpeak("\aI guessed right!");
            TXVideo(25, 4, 1000);
            // txSpeak("\aIt was a wonderful game!");
            txPlaySound ("downloads/fanfary.wav", SND_ASYNC);
            TXVideo(284, 5, 0);
        }
        else {
            txSpeak("\aOh no");
            TXVideo(41, 6, 0);
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

    txSpeak("\a%s?", cur_node->info);
    TXVideo(36, 7, 1000);
}

type_answer GiveAndCheckMyAnswer(Tree_node* cur_node) {
    assert(cur_node);

    txSpeak("\aSo, my answer: %s\n", cur_node->info);
    TXVideo(40, 8, 2000);

    txSpeak("\aIs it right?");
    TXVideo(26, 2, 1500);

    return GetAnswerYesNo();
}

Tree_status AskAndAddRightAnswer(Akinator* akinator, Tree_node* cur_node) {
    assert(cur_node);
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    txSpeak("\aWhat is the right answer?");
    TXVideo(139, 9, 2000);

    char* right_answer = ReadAnswer();

    if (right_answer == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR);

    txSpeak("\aAsk, please, how is %s different from %s. It...?\n", right_answer, cur_node->info);
    TXVideo(139, 9, 2000);

    char* difference = ReadAnswer();

    if (difference == NULL)
        TREE_CHECK_AND_RETURN_ERRORS(READ_ERROR,    free(right_answer));

    TREE_CHECK_AND_RETURN_ERRORS(InsertTwoLeaves(&akinator->tree, &cur_node, right_answer, difference),     free(right_answer);
                                                                                                            free(difference););

    txSpeak("\aThanks! It's very interesting. I remembered it");
    TXVideo(26, 2, 2000);

    free(right_answer);
    free(difference);

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    return SUCCESS;
}

Tree_status PathToCharacter(Akinator* akinator) {
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree));

    txSpeak("\aEnter name of the character");
    TXVideo(26, 2, 2000);

    stack_t stack = {};
    char* character = NULL;
    TREE_CHECK_AND_RETURN_ERRORS(ReadAndDefinitionCharacter(akinator, &character, &stack));

    txSpeak("\aPath to your character");
    TXVideo(26, 2, 1000);

    size_t cnt_connecting_words = sizeof(connecting_words) / sizeof(connecting_words[0]);

    txSpeak("\a%s", character);
    TXVideo(40, 8, 0);
    
    Tree_node* cur_node = akinator->tree.root;

    for (size_t i = 0; i < stack.size - 1; ++i) {
        size_t num_of_word = (size_t)rand() % cnt_connecting_words;

        PrintCurNode(&stack, i, cur_node);

        TXVideo(40, 8, 0);

        txSpeak("%s", connecting_words[num_of_word]);

        TREE_CHECK_AND_RETURN_ERRORS(MoveToNextNode(&stack, i, &cur_node));
    }
    // printf last sign, without common
    if (stack.data[stack.size - 1] == LEFT_NODE) {
        txSpeak("\a%s", cur_node->info);
    }
    else {
        txSpeak("\aNot %s", cur_node->info);
    }
    TXVideo(40, 8, 0);

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

    txSpeak("\aEnter name of the first character");
    TXVideo(26, 2, 2000);
    char* first_character = NULL;
    stack_t first_stack = {};
    TREE_CHECK_AND_RETURN_ERRORS(ReadAndDefinitionCharacter(akinator, &first_character, &first_stack));

    txSpeak("\aEnter name of the second character");
    TXVideo(26, 2, 2000);
    char* second_character = NULL;
    stack_t second_stack = {};
    TREE_CHECK_AND_RETURN_ERRORS(ReadAndDefinitionCharacter(akinator, &second_character, &second_stack));

    txSpeak("\aNow you can listen to common signs of %s and %s", first_character, second_character);
    TXVideo(40, 8, 2000);

    size_t both_size = MinSize_t(first_stack.size, second_stack.size);
    size_t cur_size = 0;

    Tree_node* cur_node = akinator->tree.root;
    for (; cur_size < both_size; ++cur_size) {
        if (first_stack.data[cur_size] != second_stack.data[cur_size]) {
            break;
        }

        PrintCurNode(&first_stack, cur_size, cur_node);
        TXVideo(40, 8, 0);

        TREE_CHECK_AND_RETURN_ERRORS(MoveToNextNode(&first_stack, cur_size, &cur_node));
    }
    printf("\n");

    Tree_node* cur_node_first = cur_node;
    Tree_node* cur_node_second = cur_node;

    txSpeak("\aNow you can listen to diffrent signs");
    TXVideo(40, 8, 2000);

    txSpeak("\aFirst character %s has such signs", first_character);
    TXVideo(40, 8, 2000);
    TREE_CHECK_AND_RETURN_ERRORS(PrintDifferentSigns(cur_node_first, &first_stack, cur_size));

    txSpeak("\aSecond character %s has such signs", second_character);
    TXVideo(40, 8, 2000);
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

    if (stack->data[index] == LEFT_NODE) {
        txSpeak("\a%s", cur_node->info);
    }
    else if (stack->data[index] == RIGHT_NODE) {
        txSpeak("\aNot %s", cur_node->info);
    }    
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

        TXVideo(40, 8, 0);

        TREE_CHECK_AND_RETURN_ERRORS(MoveToNextNode(stack, i, &cur_node));
    }
    // printf last sign, without common
    if (stack->data[stack->size - 1] == LEFT_NODE) {
        txSpeak("\a%s", cur_node->info);
    }    
    else {
        txSpeak("\aNot %s", cur_node->info);
    }

    TXVideo(40, 8, 0);

    return SUCCESS;
}

Tree_status SaveTree(Akinator* akinator) {
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    txSpeak("\aPlease, enter the name of the file where the tree will be saved");
    TXVideo(26, 2, 500);
    TXVideo(26, 2, 500);
    TXVideo(26, 2, 1500);

    char* file_name = ReadAnswer();

    if (file_name == NULL) {
        txSpeak("\aSorry, I couldn't do it, I have some problems. Try later please");
        TXVideo(100, 3, 2000);
    }
    else
        TREE_CHECK_AND_RETURN_ERRORS(CreateTreeFile(&akinator->tree, file_name));

    txSpeak("\aAll right, tree is saved in your file %s", file_name);
    TXVideo(25, 4, 500);
    TXVideo(25, 4, 500);
    TXVideo(25, 4, 1000);

    free(file_name);

    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    return SUCCESS;
}

Tree_status UploadTree(Akinator* akinator) {
    TREE_CHECK_AND_RETURN_ERRORS(TreeVerify(&akinator->tree)); 

    txSpeak("\aPlease, enter the name of the file from which the tree will be uploaded");
    TXVideo(26, 2, 500);
    TXVideo(26, 2, 500);
    TXVideo(26, 2, 1500);

    char* file_name = ReadAnswer();

    if (file_name == NULL) {
        txSpeak("\aSorry, I couldn't do it, I have some problems. Try later please");
        TXVideo(100, 3, 2000);
    }
    else
        TREE_CHECK_AND_RETURN_ERRORS(ReadTree(akinator, file_name));

    txSpeak("\aAll right, tree is uploaded from your file %s\n", file_name);
    TXVideo(25, 4, 500);
    TXVideo(25, 4, 500);
    TXVideo(25, 4, 1000);

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