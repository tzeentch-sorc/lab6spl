#include <stdio.h>
#include "mem.h"
int main() {
    size_t size = 39352;
    int* arr = _malloc(sizeof(int)*size);
    for(size_t i = 0; i < size; i++){
        arr[i] = (int)i;
    }

    for(size_t i = 0; i < size; i++){
        printf("%d_\n", arr[i]);
    }

    size_t size2 = 12345;
    int* arr2 = _malloc(sizeof(int)*size2);

    int* var = _malloc(sizeof(int));

    showAllocatedBlocks();
    printf("After calling  _free:\n");
    _free(arr);
    showAllocatedBlocks();






    return 0;
}