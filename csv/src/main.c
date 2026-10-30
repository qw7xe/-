#include <stdio.h> 
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../include/cli.h" // обрабатывает аргументы командной строки 
#include "../include/csv.h" // читает csv файл и формирует таблицу
#include "../include/stats.h" // проходит по таблице и собирает статистику

typedef enum {
    SUCCESS,
    RUNTIME_ERROR,
    CLI_ERROR,
} ExitCode; // код, возвращаемый функцией main


int main (int argn, char** argv) {
    
    CLI_data cli_data = command_line_parse(argn, argv); // возвращает пару значений: путь до файла и режим работы 
    
    if (cli_data.mode == ERROR_MODE) { // в командной строке неверная команда 
        fprintf(stderr, "Invalid command!\n");
        return CLI_ERROR;
    }

    if (cli_data.mode == HELP_MODE) { // аргумент --help
        print_help(); // напечатать справку
        return SUCCESS;
    }

    Table* table = parse_table(cli_data.filepath); // открыть и прочитать файл, создать и вернуть таблицу
    if (table == NULL) { // по какой-то причине таблица не создана
        fprintf(stderr, "Parsing error!\n");
        return RUNTIME_ERROR;
    }

    if (cli_data.mode == SCHEMA_MODE) { // аргумент schema
        if (!schema(table)) { // вызвать функцию schema и, если вернулся 0, вывести ошибку
            fprintf(stderr, "Calculation error!\n");
            goto fail; // перейти к блоку fail
        }
    } else if (cli_data.mode == STATS_MODE) { // аргумент stats
        if (!stats(table)) { // вызвать функцию stats и, если вернулся 0, вывести ошибку
            fprintf(stderr, "Calculation error!\n");
            goto fail;
        }
    }
    else {
        fprintf(stderr, "Unknown mode!\n"); // практически невозможно, но лучше прописать ошибку на будущее
        goto fail;
    }

    free_table(table); // очистить память таблицы с помощью специальной функции
    return SUCCESS;

fail:
    free_table(table);
    return RUNTIME_ERROR;
}