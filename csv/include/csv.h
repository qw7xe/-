#pragma once
#include <stddef.h> // чтобы использовать size_t - тип для хранения размеров массивов

typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
} Column_type; // тип столбца

typedef struct {
    void* ptr; // указатель на данные произвольного типа (т.к. тип может поменяться в процессе чтения с int на double)
    Column_type type; // тип
    size_t size; // фактическое число элементов
    size_t capacity; // на сколько элементов выделено памяти (чтобы не выделять при каждом добавлении)
    char* header; // заголовок (название) столбца
} Column; // сам столбец

typedef struct {
    Column** columns; // массив столбцов 
    size_t size; // кол-во столбцов 
    size_t capacity; // на сколько столбцов выделено памяти
} Table; // вся таблица 

Table* parse_table (char* filepath); // прочитать файл и вернуть таблицу (или NULL при ошибке) 
void free_table (Table* table); // очистить память таблицы, чтобы не было утечки