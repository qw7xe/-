#pragma once // директива препроцессора (инструкция), чтобы заголовочный (.h) файл 
             // был привязан только один раз и не было ошибки на этапе линковки

typedef enum {
    ERROR_MODE,
    HELP_MODE,
    SCHEMA_MODE,
    STATS_MODE,
} Mode; // режим работы: ошибка / справка / команда schema или stats

typedef struct {
    char* filepath;
    Mode mode;
} CLI_data; // пара значений: путь до файла и режим работы

CLI_data command_line_parse (int argn, char** argv); // прочитать аргументы командной строки
void print_help (); // напечатать справку