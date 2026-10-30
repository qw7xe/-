#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../include/csv.h"


#define CELL_DELIM ',' 
#define ROW_DELIM '\n'
#define DEFAULT_CAPACITY 256

int table_realloc (Table* table) { 
    if (table == NULL) return 0;

    size_t new_capacity = table->capacity * 2;
    
    Column** temp = (Column**) realloc(table->columns, sizeof(*temp) * new_capacity);
    if (temp == NULL) return 0; 

    table->capacity = new_capacity;
    table->columns = temp;
    return 1;
}

int add_column (Table* table, char* header) { 
    if (table == NULL || header == NULL) return 0;
    if (table->size == table->capacity) if(!table_realloc(table)) return 0; 

    Column* new_column = (Column*) malloc(sizeof(*new_column)); 
    if (new_column == NULL) return 0; 
    
    long long* data = (long long*) malloc(sizeof(long long) * DEFAULT_CAPACITY); 
    if (data == NULL) { free(new_column); return 0; } 

    new_column->ptr = data;
    new_column->type = TYPE_INT;
    new_column->size = 0;
    new_column->capacity = DEFAULT_CAPACITY;
    new_column->header = header;

    table->columns[table->size] = new_column;
    table->size++;
    return 1;
}

int column_realloc (Column* column) { 
    if (column == NULL) return 0;

    size_t new_capacity = column->capacity * 2;
    size_t elem_size; 
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

int column_change_type (Column* column) { 
    if (column == NULL) return 0;

    double* temp = (double *) malloc(column->capacity * sizeof(double)); 
    if (temp == NULL) return 0;

    long long* old = (long long*) column->ptr; 
    for (size_t i = 0; i < column->size; i++) {
        temp[i] = (double) old[i]; 
    }
    
    free(column->ptr); 
    column->ptr = temp; 
    column->type = TYPE_DOUBLE;
    return 1;
}

int add_cell (Column* column, Column_type type, char* value) { 
    if (column == NULL || value == NULL) return 0;
    if (type == TYPE_DOUBLE && column->type == TYPE_INT) { 
        if (!column_change_type(column)) return 0; 
    }

    if (column->size == column->capacity) if(!column_realloc(column)) return 0; 

    if (type == TYPE_INT) ((long long*)column->ptr)[column->size] = atoll(value); 
    else if (type == TYPE_DOUBLE) ((double*)column->ptr)[column->size] = atof(value); 
    else return 0; 

    column->size++;
    return 1;
}

char** readline(FILE* file) { 
    if (file == NULL) return NULL;

    char** row = (char**) malloc(sizeof(*row) * DEFAULT_CAPACITY); 
    if (row == NULL) return NULL;
    size_t row_size = 0, row_cap = DEFAULT_CAPACITY;

    char* cell = malloc(sizeof(*cell) * DEFAULT_CAPACITY); 
    if (cell == NULL) { 
        free(row); 
        return NULL;
    }
    size_t cell_size = 0, cell_cap = DEFAULT_CAPACITY;

    int ch;
    while ((ch = fgetc(file)) != EOF && ch != ROW_DELIM) { 
        
        if (cell_size >= cell_cap) { 
            cell_cap *= 2;
            char* temp = realloc(cell, sizeof(*cell) * cell_cap);
            if (temp == NULL) goto fail;
            cell = temp;
        }

        if (ch == CELL_DELIM) { 
            cell[cell_size] = '\0';  

            if (row_size >= row_cap) { 
                row_cap *= 2;
                char** temp = realloc(row, sizeof(*row) * row_cap);
                if (!temp) goto fail;
                row = temp;
            }

            char* dup = malloc(cell_size + 1); 
            if (dup == NULL) goto fail;
            memcpy(dup, cell, cell_size + 1); 
            row[row_size] = dup; 
            row_size++;
            cell_size = 0;
        } else { 
            cell[cell_size] = (char)ch;
            cell_size++;
        }
    }

    if (ch == EOF && row_size == 0 && cell_size == 0) { 
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
    

    free(cell);

    if (row_size == row_cap) { 
        row_cap += 1;
        char** temp = realloc(row, sizeof(*row) * row_cap);
        if (temp == NULL) {
            for (size_t i = 0; i < row_size; i++) free(row[i]);
            free(row);
            return NULL;
        }
        row = temp;
    }
    row[row_size] = NULL;

    return row;

fail:
    for (size_t i = 0; i < row_size; i++) free(row[i]); 
    free(row); 
    free(cell); 
    return NULL;
}

int parse_headers (Table* table, FILE* file) { 
    if (table == NULL || file == NULL) return 0;
    
    char** headers = readline(file); 
    if (headers == NULL) return 0;

    size_t i = 0;
    while (headers[i] != NULL) { 
        if(!add_column(table, headers[i])) goto fail;  
        i++;                                          
    }

    free(headers);
    return 1;

fail:
    for (size_t j = i; headers[j] != NULL; ++j) free(headers[j]); 
    free(headers); 
    return 0;
}

int parse_data (Table* table, FILE* file) { 
    if (table == NULL || file == NULL) return 0;

    char** data = readline(file); 
    if (data == NULL) return 0;

    size_t i = 0;
    while (data[i] != NULL) { 
        Column* col = table->columns[i];
        Column_type type = col->type;
        char* value = data[i];
        size_t c = 0;

        while (value[c] != '\0') { 
            if (value[c] == '.' && col->type == TYPE_INT) { 
                if(!column_change_type(col)) goto fail; 
                type = TYPE_DOUBLE;
            }
            c++;
        }
        if (!add_cell(col, type, value)) goto fail; 
        i++;
    }

    size_t j = 0;
    while (data[j] != NULL) { free(data[j]); j++; } 
    free(data);  
    return 1;

fail:
    j = 0;
    while (data[j] != NULL) { free(data[j]); j++; }
    free(data);
    return 0;
}

void free_table (Table* table) { 
    if (table == NULL) return; 
    if (table->columns != NULL) { 
        for (size_t i = 0; i < table->size; i++) {
            if (table->columns[i] == NULL) continue; 
            free(table->columns[i]->ptr); 
            free(table->columns[i]->header); 
            free(table->columns[i]); 
        }
        free(table->columns); 
    }
    free(table); 
}

Table* parse_table (char* filepath) { 
    if (filepath == NULL) return NULL;
    
    Table* table = (Table*) malloc(sizeof(*table)); 
    if (table == NULL) return NULL; 

    Column** columns = (Column**) malloc(sizeof(*columns) * DEFAULT_CAPACITY); 
    if (columns == NULL) {
        free(table); 
        return NULL;
    }

    table->columns = columns;
    table->size = 0;
    table->capacity = DEFAULT_CAPACITY;

    FILE* file = fopen(filepath, "r"); 
    if (file == NULL) {
        free_table(table); 
                             
        return NULL;
    }

    if (!parse_headers(table, file)) { 
        free_table(table);             
        fclose(file);                  
        return NULL;
    }

    while (1) if (!parse_data(table, file)) break; // пока читаются строки записываем данные в таблицу 

    fclose(file); // закрываем файл
    return table; // возвращаем указатель на готовую таблицу
} 
