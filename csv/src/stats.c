#include<stdio.h>
#include<math.h>

#include "../include/stats.h"


long double normalize (long double v) {return v == 0.0L ? 0.0L : v;} 

int schema (Table* table) {
    if (table == NULL || table->columns == NULL) return 0; 
    size_t rows = table->columns[0]->size;
    size_t columns = table->size;
    
    printf("rows %zu\n", rows);
    printf("columns %zu\n", columns);

    for (size_t i = 0; i < columns; i++) {
        char* header = table->columns[i]->header;
        Column_type type = table->columns[i]->type;
        printf("%zu \"%s\" %s\n", i, header, (type == TYPE_INT) ? "INT" : "DOUBLE");
       
    }

    return 1;
}
int stats (Table* table) {
    if (table == NULL || table->columns == NULL) return 0;
    size_t rows = table->columns[0]->size;
    size_t columns = table->size;

    printf("rows %zu\n", rows);
    printf("columns %zu\n", columns);

    for (size_t i = 0; i < columns; i++) {
        Column* col = table->columns[i]; 
        
        char* header = col->header; 
        Column_type type = col->type;
        
        size_t pos = 0, neg = 0, even = 0, integer = 0; 

        if (type == TYPE_INT) {
            long long min = ((long long*)col->ptr)[0]; 
            long long max = ((long long*)col->ptr)[0]; 
            long double sum = 0; 

            for (size_t j = 0; j < rows; ++j) {
                long long value = ((long long*)col->ptr)[j];
                if (value < min) min = value;
                if (value > max) max = value;
                sum += (long double)value; 
                pos += (value > 0);
                neg += (value < 0);
                even += (value % 2 == 0); 
            }

            printf("column %zu\n", i); // у size_t спецификатор %zu
            printf("name \"%s\"\n", header);
            printf("type %s\n", (type == TYPE_INT) ? "INT" : "DOUBLE");
            printf("nulls 0\n"); 
            printf("min %lld\n", min);
            printf("max %lld\n", max);
            printf("mean %.6Lf\n", normalize(sum / (long double)rows)); 
            printf("positive %zu\n", pos);
            printf("negative %zu\n", neg);
            printf("even %zu\n\n", even);
 
        } else {
            double min = ((double*)col->ptr)[0]; 
            double max = ((double*)col->ptr)[0];
            long double sum = 0;

            for (size_t j = 0; j < rows; ++j) {
                double value = ((double*)col->ptr)[j];
                if (value < min) min = value;
                if (value > max) max = value;
                sum += (long double)value;
                pos += (value > 0);
                neg += (value < 0);
                integer += (value == floor(value)); 
            }

            printf("column %zu\n", i);
            printf("name \"%s\"\n", header);
            printf("type %s\n", (type == TYPE_INT) ? "INT" : "DOUBLE");
            printf("nulls 0\n");
            printf("min %.6f\n", (double)normalize(min)); 
            printf("max %.6f\n", (double)normalize(max));
            printf("mean %.6Lf\n", normalize(sum / (long double)rows));
            printf("positive %zu\n", pos);
            printf("negative %zu\n", neg);
            printf("integer %zu\n\n", integer);

        }
    }
    return 1;   
}
