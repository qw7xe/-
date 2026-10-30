#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../include/csv.h"

// в макросах объявляем разделители и стандартный размер массива (чтобы не заводить глобальные переменные)
#define CELL_DELIM ',' 
#define ROW_DELIM '\n'
#define DEFAULT_CAPACITY 256

int table_realloc (Table* table) { // увеличить память под указатели на столбцы в два раза
    if (table == NULL) return 0;

    size_t new_capacity = table->capacity * 2;
    
    Column** temp = (Column**) realloc(table->columns, sizeof(*temp) * new_capacity);
    if (temp == NULL) return 0; // если ошибка выделения памяти (аллокации) вернуть 0

    table->capacity = new_capacity;
    table->columns = temp;
    return 1;
}

int add_column (Table* table, char* header) { // добавить столбец с заголовком header последним в таблицу
    if (table == NULL || header == NULL) return 0;
    if (table->size == table->capacity) if(!table_realloc(table)) return 0; // если место закончилось, выделить больше памяти
                                                                            // и если не получилось вернуть 0  

    Column* new_column = (Column*) malloc(sizeof(*new_column)); // выделить память под структуру столбца
    if (new_column == NULL) return 0; // стандартная проверка после каждого выделения
    
    long long* data = (long long*) malloc(sizeof(long long) * DEFAULT_CAPACITY); // выделить память под значения столбца
    if (data == NULL) { free(new_column); return 0; } // не забываем очистить структуру столбца

    new_column->ptr = data;
    new_column->type = TYPE_INT;
    new_column->size = 0;
    new_column->capacity = DEFAULT_CAPACITY;
    new_column->header = header;

    table->columns[table->size] = new_column;
    table->size++;
    return 1;
}

int column_realloc (Column* column) { // увеличить память под значения столбца в два раза
    if (column == NULL) return 0;

    size_t new_capacity = column->capacity * 2;
    size_t elem_size; // сколько байт на один элемент
    switch (column->type) {
        case TYPE_INT: elem_size = sizeof(long long); break;
        case TYPE_DOUBLE: elem_size = sizeof(double); break;
        default: return 0;
    }
    void* temp = realloc(column->ptr, new_capacity * elem_size);
    if (temp == NULL) return 0;

    column->capacity = new_capacity;
    column->ptr = temp;
    return 1;
}       

int column_change_type (Column* column) { // поменять тип столбца с TYPE_INT на TYPE_DOUBLE (перевыделить память)
    if (column == NULL) return 0;

    double* temp = (double *) malloc(column->capacity * sizeof(double)); // создали новый массив данных
    if (temp == NULL) return 0;

    long long* old = (long long*) column->ptr; // запомнили указатель на старые данные
    for (size_t i = 0; i < column->size; i++) {
        temp[i] = (double) old[i]; // по одному скопировали значения
    }
    
    free(column->ptr); // очистили старую память
    column->ptr = temp; // соединили столбец с новыми данными 
    column->type = TYPE_DOUBLE;
    return 1;
}

int add_cell (Column* column, Column_type type, char* value) { // добавить значение value в столбец column типа type
    if (column == NULL || value == NULL) return 0;
    if (type == TYPE_DOUBLE && column->type == TYPE_INT) { // нужно поменять тип столбца и перевыделить память
        if (!column_change_type(column)) return 0; // если не получилось вернуть 0
    }

    if (column->size == column->capacity) if(!column_realloc(column)) return 0; // нужно перевыделить память под новые элементы (не меняя тип)

    if (type == TYPE_INT) ((long long*)column->ptr)[column->size] = atoll(value); // встроенная функция, превращает строку (массив char) в long long
    else if (type == TYPE_DOUBLE) ((double*)column->ptr)[column->size] = atof(value); // встроенная функция, превращает строку (массив char) в double
    else return 0; // практически невозможно, но лучше оставить обработку ошибки на будущее

    column->size++;
    return 1;
}

char** readline(FILE* file) { // прочитать строку из файла, разделить ее на подстроки по разделителю и вернуть массив этих подстрок
    if (file == NULL) return NULL;

    char** row = (char**) malloc(sizeof(*row) * DEFAULT_CAPACITY); // память под этот массив
    if (row == NULL) return NULL;
    size_t row_size = 0, row_cap = DEFAULT_CAPACITY;

    char* cell = malloc(sizeof(*cell) * DEFAULT_CAPACITY); // память под текущую подстроку
    if (cell == NULL) { 
        free(row); // не забываем очистить массив
        return NULL;
    }
    size_t cell_size = 0, cell_cap = DEFAULT_CAPACITY;

    int ch;
    while ((ch = fgetc(file)) != EOF && ch != ROW_DELIM) { // получаем новый символ и проверяем на конец строки или всего файла
        
        if (cell_size >= cell_cap) { // если память под подстроку заканчивается, перевыделяем
            cell_cap *= 2;
            char* temp = realloc(cell, sizeof(*cell) * cell_cap);
            if (temp == NULL) goto fail;
            cell = temp;
        }

        if (ch == CELL_DELIM) { // наткнулись на разделитель клеток (запятую)
            cell[cell_size] = '\0'; // добавили закрывающий символ в подстроку 

            if (row_size >= row_cap) { // если памяти в массиве не хватает, перевыделили
                row_cap *= 2;
                char** temp = realloc(row, sizeof(*row) * row_cap);
                if (!temp) goto fail;
                row = temp;
            }

            char* dup = malloc(cell_size + 1); // создали новый указатель на подстроку, чтобы не потерять данные при очистке
            if (dup == NULL) goto fail;
            memcpy(dup, cell, cell_size + 1); // скопировали данные в новый указатель
            row[row_size] = dup; // записали в массив
            row_size++;
            cell_size = 0;
        } else { // если обычный символ, просто добавляем в подстроку
            cell[cell_size] = (char)ch;
            cell_size++;
        }
    }

    if (ch == EOF && row_size == 0 && cell_size == 0) { // если нет ни одной клетки, очищаем память
        free(cell);
        free(row);
        return NULL;
    }

    // ОТСЮДА
    if (cell_size >= cell_cap) { 
        cell_cap *= 2;
        char* temp = realloc(cell, sizeof(*cell) * cell_cap);
        if (temp == NULL) goto fail;
        cell = temp;
    }
    cell[cell_size] = '\0';

    if (row_size >= row_cap) {
        row_cap *= 2;
        char** temp = realloc(row, sizeof(*row) * row_cap);
        if (temp == NULL) goto fail;
        row = temp;
    }

    char* dup = malloc(cell_size + 1);
    if (dup == NULL) goto fail;
    memcpy(dup, cell, cell_size + 1);
    row[row_size] = dup;
    row_size++;
    // И ДОСЮДА повторяем операции для последней подстроки (так как в конце строки файла нет запятой)

    free(cell);

    if (row_size == row_cap) { // если еще один элемент в массив не поместится, перевыделяем память
        row_cap += 1;
        char** temp = realloc(row, sizeof(*row) * row_cap);
        if (temp == NULL) {
            for (size_t i = 0; i < row_size; i++) free(row[i]);
            free(row);
            return NULL;
        }
        row = temp;
    }
    row[row_size] = NULL; // и записываем этот последний элемент - NULL

    return row;

fail:
    for (size_t i = 0; i < row_size; i++) free(row[i]); // очищаем каждуб подстроку
    free(row); // очищаем массив
    free(cell); // и текущуб подстроку 
    return NULL;
}

int parse_headers (Table* table, FILE* file) { // прочитать строчку из file и для каждого заголовка создать столбец
    if (table == NULL || file == NULL) return 0;
    
    char** headers = readline(file); // получить массив строк (указатель на указатели на char), разбив строку file по разделителю
    if (headers == NULL) return 0;

    size_t i = 0;
    while (headers[i] != NULL) { // пока есть заголовки
        if(!add_column(table, headers[i])) goto fail; // добавить в таблицу столбец TYPE_INT с таким заголовком 
        i++;                                          // иначе перейти к блоку fail (очистка и возврат 0)
    }

    free(headers);
    return 1;

fail:
    for (size_t j = i; headers[j] != NULL; ++j) free(headers[j]); // очистить каждый заголовок
    free(headers); // очистить список заголовков
    return 0;
}

int parse_data (Table* table, FILE* file) { // прочитать строку из файла и каждое значение записать в нужный столбец 
    if (table == NULL || file == NULL) return 0;

    char** data = readline(file); // получить нужный массив строк
    if (data == NULL) return 0;

    size_t i = 0;
    while (data[i] != NULL) { // пока есть строка 
        Column* col = table->columns[i];
        Column_type type = col->type;
        char* value = data[i];
        size_t c = 0;

        while (value[c] != '\0') { // перебираем все символы строки
            if (value[c] == '.' && col->type == TYPE_INT) { // если выполняются условия вещественного числа
                if(!column_change_type(col)) goto fail; // пытаемся поменять тип текущего столбца, если не получается, переходим к блоку fail
                type = TYPE_DOUBLE;
            }
            c++;
        }
        if (!add_cell(col, type, value)) goto fail; // пытаемся добавить клетку (значение в столбец) в таблицу, иначе переходим к блоку fail
        i++;
    }

    size_t j = 0;
    while (data[j] != NULL) { free(data[j]); j++; } // очищаем каждую строчку
    free(data); // очищаем вспомогательный массив строк
    return 1;

fail:
    j = 0;
    while (data[j] != NULL) { free(data[j]); j++; }
    free(data);
    return 0;
}

void free_table (Table* table) { // специализированная очистка структуры Table
    if (table == NULL) return; // уже очищена - ничего не делаем
    if (table->columns != NULL) { // столбцы тоже существуют
        for (size_t i = 0; i < table->size; i++) {
            if (table->columns[i] == NULL) continue; // какого-то столбца нет - пропускаем (на практике быть не должно)
            free(table->columns[i]->ptr); // очищаем массив значений
            free(table->columns[i]->header); // очищаем заголовок
            free(table->columns[i]); // очищвем весь столбец
        }
        free(table->columns); // очищаем массив указателей на столбцы 
    }
    free(table); // очищаем память самой таблицы
}

Table* parse_table (char* filepath) { // главная функция, читающая файл и создающая таблицу
    if (filepath == NULL) return NULL;
    
    Table* table = (Table*) malloc(sizeof(*table)); // выделяем память под структуру таблица
    if (table == NULL) return NULL; // при ошибке возвращаем NULL

    Column** columns = (Column**) malloc(sizeof(*columns) * DEFAULT_CAPACITY); // выделяем память под указатели на столбцы
    if (columns == NULL) {
        free(table); // при ошибке очищаем структуру таблица
        return NULL;
    }

    table->columns = columns;
    table->size = 0;
    table->capacity = DEFAULT_CAPACITY;

    FILE* file = fopen(filepath, "r"); // пытаемся открыть файл
    if (file == NULL) {
        free_table(table); // при ошибке очищаем таблицу (отдельно columns очищать не надо, 
                           // так как они уже привязаны в table->columns и free_table все правильно очистит)  
        return NULL;
    }

    if (!parse_headers(table, file)) { // прочитать первую строчку файла и взять оттуда заголовки 
        free_table(table);             // если не получилось, выполнить очистку
        fclose(file);                  // и закрыть файл
        return NULL;
    }

    while (1) if (!parse_data(table, file)) break; // пока читаются строки записываем данные в таблицу 

    fclose(file); // закрываем файл
    return table; // возвращаем указатель на готовую таблицу
} 