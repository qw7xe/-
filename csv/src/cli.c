#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cli.h"

CLI_data command_line_parse (int argn, char** argv) {
    CLI_data result = {.filepath = NULL, .mode = ERROR_MODE}; // инициализация (значения по умолчанию)
    // argn число аргументов, при этом argv[0] это сам вызов ./run, поэтому нужных нам, осмысленных аргументов на один меньше 
    if (argn == 2 && strcmp(argv[1], "--help") == 0) {
        result.mode = HELP_MODE;

    } else if (argn == 3) {

        if (strcmp(argv[1], "schema") == 0) 
            result.mode = SCHEMA_MODE;
        
        else if (strcmp(argv[1], "stats") == 0)
            result.mode = STATS_MODE;
        
        if (result.mode != ERROR_MODE) // путь к файлу только если вызвана одна из двух команд
            result.filepath = argv[2];
    }
    return result;
}

void print_help (void) {
    printf("Usage: ./run [command] <file.csv>\n"); // Использование: ./run [команда] <файл.csv>
    printf("\nCommands: schema - general info\n"); // schema - общая информация
    printf("          stats  - detailed info\n"); // stats - подробная информация
    printf("\nFor help: ./run --help\n"); // для справки: ./run --help
}