#include "cache.h"
#include <stdio.h>

#define MAX_SIZE 1024

typedef long array_t[MAX_SIZE * MAX_SIZE];
static cache_t *cache;

array_t test_array;

/*
 * Write data to the array, bypassing the "cache".
 */
void fillArray(array_t a) {

    int i, j;

    for (i = 0; i < MAX_SIZE; i++)
        for (j = 0; j < MAX_SIZE; j++)
            a[i * MAX_SIZE + j] = (i + 1) * (j + 1);
}

long sumA(array_t a, int rows, int cols) {

    int i, j;
    long sum = 0;

    for (i = 0; i < rows; i++)
        for (j = 0; j < cols; j++)
            sum += cache_read(cache, &a[i * cols + j]);

    return sum;
}

long sumB(array_t a, int rows, int cols) {

    int i, j;
    long sum = 0;

    for (j = 0; j < cols; j++)
        for (i = 0; i < rows; i++)
            sum += cache_read(cache, &a[i * cols + j]);

    return sum;
}

long sumC(array_t a, int rows, int cols) {

    int i, j;
    int sum = 0;

    for (j = 0; j < cols; j += 2)
        for (i = 0; i < rows; i += 2)
            sum += cache_read(cache, &a[i * cols + j]) +
                   cache_read(cache, &a[(i+1) * cols + j]) +
                   cache_read(cache, &a[i * cols + j+1]) +
                   cache_read(cache, &a[(i+1) * cols + j+1]);

    return sum;
}

void print_stats() {

    int mc = cache_miss_count(cache);
    int ac = cache_access_count(cache);

    if (ac == 0) {
        printf("The cache wasn't used.\n");
    }
    else {
        printf("Miss rate = %8.4f\n", (double) mc/ac);
    }
}

int main() {

    fillArray(test_array);

    cache = cache_new(256, 64, 1, CACHE_REPLACEMENTPOLICY_LRU);
    printf("Sum = %ld\n", sumA(test_array, 64, 64));
    print_stats();

    cache = cache_new(256, 64, 1, CACHE_REPLACEMENTPOLICY_LRU);
    printf("Sum = %ld\n", sumB(test_array, 64, 64));
    print_stats();

    cache = cache_new(256, 64, 1, CACHE_REPLACEMENTPOLICY_LRU);
    printf("Sum = %ld\n", sumC(test_array, 64, 64));
    print_stats();

    return 0;
}