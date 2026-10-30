#pragma once
#include <stddef.h> 

typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
} Column_type; 

typedef struct {
    void* ptr; 
    Column_type type; 
    size_t size; 
    size_t capacity; 
    char* header; 
} Column; 

typedef struct {
    Column** columns; 
    size_t size; 
    size_t capacity; 
} Table; 

Table* parse_table (char* filepath); 
void free_table (Table* table); 
