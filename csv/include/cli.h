#pragma once 
             

typedef enum {
    ERROR_MODE,
    HELP_MODE,
    SCHEMA_MODE,
    STATS_MODE,
} Mode; 

typedef struct {
    char* filepath;
    Mode mode;
} CLI_data; 

CLI_data command_line_parse (int argn, char** argv); 
void print_help (); 
