#pragma once

#include <stdlib.h>

#include "data.h"

typedef struct Entry {
    const char* key;
    void* value;
    struct Entry* next;
} Entry;

typedef struct {
    Entry* first;
} Bucket;

typedef struct Scope {
    Bucket* buckets;
    struct Scope* next;
} Scope;

typedef struct {
    Scope* base;
    Scope* top;
} Stack;

Scope* make_scope(Scope* next);
Stack* make_stack(void);
Entry* make_entry(const char* key, void* value);
Entry* append(Bucket* bucket, const char* key, void* value);
Entry* insert(Scope* level, const char* key, void* value);
Entry* add(Stack* stack, const char* key, void* value);
Entry* find_entry(Stack* stack, const char* key);
void* find(Stack* stack, const char* key);
Scope* push(Stack* stack);
Scope* pop(Stack* stack);
Stack* stack_copy(Stack*);
void print_stack(Stack*);