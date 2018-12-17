//
// Created by radiant on 04.12.18.
//

#ifndef LAB6_MEM_H
#define LAB6_MEM_H

#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>
#include <setjmp.h>
#include <bits/types/FILE.h>

#define  HEAP_START  ((void*)0x04040000)

#pragma pack(push, 1)
typedef struct mem  {
    struct mem* next;
    size_t capacity;
    uint8_t is_free;
} mem;
#pragma pack(pop)

void* _malloc( size_t query );
void* _calloc(size_t num, size_t size);
void* _realloc(void* ptr, size_t new_size);

void  _free( void* ptr );
void* heap_init(size_t initSize);

void showAllocatedBlocks();

void memalloc_debug_struct_info( FILE* f, struct mem* address );

void memalloc_debug_heap( FILE* f, struct mem const* ptr );
#endif //LAB6_MEM_H

