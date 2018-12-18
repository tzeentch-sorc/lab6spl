#include <stdio.h>
#include "mem.h"
int main() {
    size_t size = 1000;
    size_t size2 = 39352;
    int* arr = _malloc(sizeof(int)*size);
    for(size_t i = 0; i < size; i++){
        arr[i] = (int)i;
    }

    showAllocatedBlocks();
    printf("_____\n");
    int* var = _realloc(arr, size2);
    showAllocatedBlocks();
    printf("After calling  _free:\n");
    _free(arr);
    showAllocatedBlocks();
    return 0;
}