#include "Akinator.h"
#include "tree.h"

    // txSpeak("Во время скачивания файла произошло переключение с вай-фай на мобильный интернет. Скорость скачивания возросла в 10000 раз. Назовите название провайдера вай-фай");


int main(int, char** argv) {
    const char* dump_filename = argv[1];
    const char* directory     = argv[2];

    Akinator akinator = {};

    AkinatorCtor(&akinator, dump_filename, directory);

    StartAkinator(&akinator);

    AkinatorDtor(&akinator);

    return 0;
}