#include<stdio.h>
#include<math.h>

#include "../include/stats.h"


long double normalize (long double v) {return v == 0.0L ? 0.0L : v;} // иначе возможен вывод "-0.000000" а по условию так быть не может - убираем минус

int schema (Table* table) {
    if (table == NULL || table->columns == NULL) return 0; // проверяем, что параметры корректные
    size_t rows = table->columns[0]->size;
    size_t columns = table->size;
    
    printf("rows %zu\n", rows);
    printf("columns %zu\n", columns);

    for (size_t i = 0; i < columns; i++) {
        char* header = table->columns[i]->header;
        Column_type type = table->columns[i]->type;
        printf("%zu \"%s\" %s\n", i, header, (type == TYPE_INT) ? "INT" : "DOUBLE");
        // тернарный оператор: (условие) ? значение_при_выполнении : значение_в_противном_случае
        // можно заменить на блок if-else, но так выглядит чище
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
        Column* col = table->columns[i]; // для удобства
        
        char* header = col->header; 
        Column_type type = col->type;
        
        size_t pos = 0, neg = 0, even = 0, integer = 0; // счетчики

        if (type == TYPE_INT) {
            long long min = ((long long*)col->ptr)[0]; // long long - настоящий тип значений столбца TYPE_INT
            long long max = ((long long*)col->ptr)[0]; // который хранит весь требуемый диапазон значений (в отличие от int)
                                                       // указатель (col->ptr) изначально void*, поэтому нужно явно преобразовать в long long*
            long double sum = 0; // простого double может не хватить (вещественный тип, потому что будем делить)

            for (size_t j = 0; j < rows; ++j) {
                long long value = ((long long*)col->ptr)[j];
                if (value < min) min = value;
                if (value > max) max = value;
                sum += (long double)value; // явно преобразуем long long к long double
                pos += (value > 0);
                neg += (value < 0);
                even += (value % 2 == 0); 
            }

            printf("column %zu\n", i); // у size_t спецификатор %zu
            printf("name \"%s\"\n", header);
            printf("type %s\n", (type == TYPE_INT) ? "INT" : "DOUBLE");
            printf("nulls 0\n"); // на уровнях лабораторной 4/10 и 6/10 отсутствуют
            printf("min %lld\n", min);
            printf("max %lld\n", max);
            printf("mean %.6Lf\n", normalize(sum / (long double)rows)); // .6 чтобы было ровно 6 знаков после точки (по условию)
            printf("positive %zu\n", pos);
            printf("negative %zu\n", neg);
            printf("even %zu\n\n", even);
 
        } else { // дублируем код, можно сложнее и красивее, но разумно делать это на следующем уровне, когда появятся STRING столбцы
            double min = ((double*)col->ptr)[0]; // опять явное приведение указателя уже к double*
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
            printf("min %.6f\n", (double)normalize(min)); // нормализуем -0.000
            printf("max %.6f\n", (double)normalize(max));
            printf("mean %.6Lf\n", normalize(sum / (long double)rows));
            printf("positive %zu\n", pos);
            printf("negative %zu\n", neg);
            printf("integer %zu\n\n", integer);

        }
    }
    return 1;   
}