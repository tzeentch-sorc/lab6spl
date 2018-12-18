//
// Created by radiant on 04.12.18.
//

#include "mem.h"
#include <unistd.h>
#include <stdio.h>
#define BLOCK_MIN_SIZE 4
#define true 1
#define false 0

static mem* heap_start;

//void mem_init(mem* const block, const size_t capacity);
void* findBlock(size_t query);
void* pageAlign(const void* pointer);

void _free(void* ptr){ //given the block start(without header?)
    struct mem* ptrStruct =  (struct mem*)((size_t)ptr - sizeof(struct mem)); //calculate the header start
     ptrStruct->is_free = true;
     //merge if next block is free

     struct mem* currentBlock = heap_start->next;
     struct mem* prevBlock = heap_start;

     for(; currentBlock!= NULL; currentBlock = currentBlock->next){
         if(currentBlock->is_free && (prevBlock->is_free)){
             prevBlock->next = currentBlock->next;

             //size of merged block = prev.capacity + sizeof(header) + next.capacity
             prevBlock->capacity += currentBlock->capacity + sizeof(struct mem);
         }
         prevBlock = currentBlock;
     }
}

void* _malloc(size_t query){
    int val = (int) query;//saved query value

    //if query is less than 4 bytes, shall allocate 4 bytes
    if(val < BLOCK_MIN_SIZE) query = BLOCK_MIN_SIZE;

    if(heap_start != HEAP_START)
        heap_init(query);

    void* block = findBlock(query);
//    FILE *ptr;
//    ptr = fopen("debugRes.txt", "w");
//    memalloc_debug_heap(ptr, heap_start);
    return block;
}


void* findBlock(size_t query){
    struct mem* currentBlock = heap_start; //both blocks at heap start at first
    struct mem* prevBlock = heap_start;

    for(; currentBlock!= NULL; currentBlock = currentBlock->next){
        if((currentBlock->is_free) == true){
            if((currentBlock->capacity) > query) {
                //if block is free and its size is more than we need
                //we insert query bytes in memory and find where second block will start
                //it starts on address of currentBlock(which is first) + sizeof new header + query bytes
                struct mem *secondBlock = (struct mem *) ((size_t)currentBlock + sizeof(struct mem) + query);
                size_t capacitySecondBlock = currentBlock->capacity - sizeof(struct mem) - query;
                secondBlock->capacity = capacitySecondBlock;
                secondBlock->is_free = true;
                secondBlock->next = currentBlock->next;

                currentBlock->capacity = query;
                currentBlock->next = secondBlock;
                currentBlock->is_free = false;

                return (void *) ((size_t) currentBlock + sizeof(struct mem));
            }
        }

        prevBlock = currentBlock;
    }
    //if we didn`t find any free or  block of suitable size
    size_t requiredMemory = query - prevBlock->capacity; //last block(how much we need to add to the last block)
    // to insert query

    size_t size = (size_t)pageAlign((void*)requiredMemory);
    void* address = (void*)((size_t)prevBlock + sizeof(struct mem) + prevBlock->capacity);
    void* newPage = mmap(address, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if(newPage != MAP_FAILED){
        struct mem* secondBlock = (struct mem*)((size_t)prevBlock + sizeof(struct mem) + query);
        secondBlock->is_free = true;
        secondBlock->capacity = size - requiredMemory;
        secondBlock->next = NULL;
        prevBlock->capacity = query;
        prevBlock->is_free = false;
        prevBlock->next = secondBlock;

        return (void*)((size_t)prevBlock + sizeof(struct mem));
    }

    return NULL; //if a total failure
}

//page alignment
void* pageAlign(const void* pointer){
    const size_t pageSize = (size_t)getpagesize();//num of bytes on page
    size_t address = (size_t)pointer; //starting on this address
    address = address - address % pageSize + pageSize; //offset
    return (void*)address;
}

void * heap_init(size_t initSize){
    //initializing start of the heap
    size_t size = (size_t)pageAlign((void *)(initSize + sizeof(struct mem)));
    heap_start = (struct mem*)mmap(HEAP_START, size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    if(heap_start == NULL) return NULL;

    //initializing mem (chain of blocks, creating first one)
    heap_start->next = NULL;
    heap_start->capacity = (size_t)(size - sizeof(struct mem));
    heap_start->is_free = true;

    return heap_start;
}




void showAllocatedBlocks(){

struct mem* current = heap_start;
for(; current != NULL; current = current->next){
    printf("%p\n", current);
    }
}

void memalloc_debug_struct_info( FILE* f, struct mem* address ) {
    size_t i;
    fprintf( f, "start: %p\nsize: %lu\nis_free: %d\n",
             (void*)address,
             address->capacity,
             address->is_free );
    for ( i = 0; i < BLOCK_MIN_SIZE && i < address-> capacity;
          ++i )
        fprintf( f, "%hhX",
                 ((char*)address)[ sizeof( struct mem ) + i ] );
    putc( '\n', f );
}

void memalloc_debug_heap( FILE* f, struct mem const* ptr ) {
    for( ; ptr; ptr = ptr->next )
        memalloc_debug_struct_info( f, ptr );
    }

void* _calloc(size_t num, size_t size){
    return _malloc(size*num);
}

void* _memcpy(void* src, void* dest, size_t count){
    int* srcPtr = src;
    int* destPtr = dest;

    for(size_t i = 0; i < count; i++){
        *destPtr = *srcPtr;
        destPtr++;
        srcPtr++;
    }
}

void* _realloc(void* ptr, size_t new_size){
    struct mem* blocks = (struct mem*) ((size_t)ptr - sizeof(mem));
    blocks->is_free = true;
    if(blocks->capacity > new_size){
        struct mem* new = _malloc(new_size);
        _memcpy(new, ptr, sizeof(ptr));
        _free(ptr);
        return new;
    }
    else{
        return _malloc(new_size);
    }

}
