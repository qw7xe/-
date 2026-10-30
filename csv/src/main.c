#include <stdio.h> 
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../include/cli.h" 
#include "../include/csv.h" 
#include "../include/stats.h" 

typedef enum {
    SUCCESS,
    RUNTIME_ERROR,
    CLI_ERROR,
} ExitCode; // код, возвращаемый функцией main


int main (int argn, char** argv) {
    
    CLI_data cli_data = command_line_parse(argn, argv); 
    
    if (cli_data.mode == ERROR_MODE) { 
        fprintf(stderr, "Invalid command!\n");
        return CLI_ERROR;
    }

    if (cli_data.mode == HELP_MODE) { 
        print_help(); // напечатать справку
        return SUCCESS;
    }

    Table* table = parse_table(cli_data.filepath); 
    if (table == NULL) { 
        fprintf(stderr, "Parsing error!\n");
        return RUNTIME_ERROR;
    }

    if (cli_data.mode == SCHEMA_MODE) { 
        if (!schema(table)) { 
            fprintf(stderr, "Calculation error!\n");
            goto fail; 
        }
    } else if (cli_data.mode == STATS_MODE) { 
        if (!stats(table)) { 
            fprintf(stderr, "Calculation error!\n");
            goto fail;
        }
    }
    else {
        fprintf(stderr, "Unknown mode!\n"); 
        goto fail;
    }

    free_table(table); 
    return SUCCESS;

fail:
    free_table(table);
    return RUNTIME_ERROR;
}
