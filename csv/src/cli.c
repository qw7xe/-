#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cli.h"

CLI_data command_line_parse (int argn, char** argv) {
    CLI_data result = {.filepath = NULL, .mode = ERROR_MODE}; 
    if (argn == 2 && strcmp(argv[1], "--help") == 0) {
        result.mode = HELP_MODE;

    } else if (argn == 3) {

        if (strcmp(argv[1], "schema") == 0) 
            result.mode = SCHEMA_MODE;
        
        else if (strcmp(argv[1], "stats") == 0)
            result.mode = STATS_MODE;
        
        if (result.mode != ERROR_MODE) 
            result.filepath = argv[2];
    }
    return result;
}

void print_help (void) {
    printf("Usage: ./run [command] <file.csv>\n"); 
    printf("\nCommands: schema - general info\n"); 
    printf("          stats  - detailed info\n"); 
    printf("\nFor help: ./run --help\n"); 
}
