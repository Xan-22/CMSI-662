#include <stdio.h>
#include <stdlib.h>

static int ARR_SIZE = 5;
static int INDEX_TO_ACCESS = 3;

int *make_arr(int size)
{
    int *arr = (int *)malloc(ARR_SIZE * sizeof(int));
    if (arr == NULL) {
        puts("Memory allocation failed\n");
        return NULL;
    }

    for (int i = 0; i < ARR_SIZE; i++) {
        arr[i] = i + 1;
    }
    
    return arr;
}

int access_arr(int *arr, int size, int index)
{
    if (index >= 0 && index < size) {
        return arr[index];
    } else {
        puts("Index out of bounds\n");
        return 1;
    }
}

int main(void)
{
    int *arr = make_arr(ARR_SIZE);
    if (arr == NULL) {
        return 1;
    }
    int val = access_arr(arr, ARR_SIZE, INDEX_TO_ACCESS);
    printf("Element %d is: %d\n", INDEX_TO_ACCESS, val);
    free(arr);
    return 0;
}