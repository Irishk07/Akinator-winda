#ifndef AKINATOR_H_
#define AKINATOR_H_

#include "common.h"
#include "tree.h"
#include "stack.h"


const int DEFAULT_SIZE = 128;
const int CNT_ATTEMPTS = 5;


enum type_answer {
    YES          = 0,
    NO           = 1,
    WRONG_ANSWER = 2,
    ERROR_ANSWER = 3,
};

enum type_options {
    PLAY_AKINATOR      = 1,
    PATH_CHARACTER     = 2,
    COMPARE_CHARACTERS = 3,
    SAVE_TREE          = 4,
    UPLOAD_TREE        = 5,
    END_AKINATOR       = 6,
    WRONG_OPTION       = 7
};

enum type_path {
    LEFT_NODE  = 0,
    RIGHT_NODE = 1,
};


const char* const connecting_words[] = {"so", "more than that", "also", "in addition", "besides"};


void TXVideo(int cnt_shots, int number_video, int sleep);

void TXImage(const char* filename);


Tree_status AkinatorCtor(Akinator* akinator, const char *dump_filename, const char *directory);

Tree_status StartAkinator(Akinator* akinator);

type_answer GetAnswerYesNo();

char* ReadAnswer();

Tree_status ChooseOption(Akinator* akinator);

type_options ReadSelectedOption();

Tree_status PlayAkinator(Akinator* akinator, Tree_node* cur_node);

void AskQuestion(Tree_node* cur_node);

type_answer GiveAndCheckMyAnswer(Tree_node* cur_node);

Tree_status AskAndAddRightAnswer(Akinator* akinator, Tree_node* cur_node);

Tree_status PathToCharacter(Akinator* akinator);

Tree_status ReadAndDefinitionCharacter(Akinator* akinator, char** character, stack_t* stack);

Tree_status DefinitionOfCharacter(Akinator* akinator, stack_t* stack, char** character);

Tree_node* FindCharacter(stack_t* stack, Tree_node* tree_node, type_t character);

Tree_status CompareTwoCharacters(Akinator* akinator);

size_t MinSize_t(size_t first_value, size_t second_value);

void PrintCurNode(stack_t* stack, size_t index, Tree_node* cur_node);

Tree_status MoveToNextNode(stack_t* stack, size_t index, Tree_node** cur_node);

Tree_status PrintDifferentSigns(const char* first_character, const char* second_character, 
                                stack_t* first_stack, stack_t* second_stack, Tree_node* cur_node, size_t cur_size);

Tree_status PrintDifferentSignsOneCharacter(Tree_node* cur_node, stack_t* stack, size_t cur_size);

Tree_status SaveTree(Akinator* akinator);

Tree_status UploadTree(Akinator* akinator);

Tree_status ReadTree(Akinator* akinator, const char* file_with_tree);

Tree_status ReadNode(Akinator* akinator, Tree_node** tree_node, char** current_pos, Tree_node* parent);

Tree_status AkinatorDtor(Akinator* akinator);

void AkinatorNodeDtor(Akinator* akinator, Tree_node* tree_node);


#endif // AKINATOR_H_