#include "stack.h"

#define NUMBER_OF_BUCKETS 10

/* Source code for the djb2 hashing algorithm (the following function only)
 * from http://www.cse.yorku.ca/~oz/hash.html */
unsigned long hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */
    return hash;
}

Stack* stack_copy(Stack* stack) {
    Stack* new = make_stack();
    Scope** scope = &new->top;
    for (Scope* level = stack->top; level; level = level->next) {
        *scope = make_scope(NULL);
        (*scope)->buckets = level->buckets;
        new->base = *scope;
        scope = &(*scope)->next;
    }
    return new;
}

Scope* make_scope(Scope* next) {
    Scope* level = malloc(sizeof(Scope));
    level->buckets = calloc(NUMBER_OF_BUCKETS, sizeof(Bucket));
    level->next = next;
    return level;
}

Stack* make_stack(void) {
    Stack* stack = malloc(sizeof(Stack));
    stack->top = stack->base = NULL;
    return stack;
}

Entry* make_entry(const char* key, void* value) {
    Entry* entry = malloc(sizeof(Entry));
    entry->key = key;
    entry->value = value;
    entry->next = NULL;
    return entry;
}

Bucket* get_bucket(Bucket* buckets, const char* key) {
    return buckets + (hash(key) % NUMBER_OF_BUCKETS);
}

Entry* append(Bucket* bucket, const char* key, void* value) {
    Entry* entry = NULL, * new = make_entry(key, value);
    for (entry = bucket->first; entry && entry->next; entry = entry->next) {
        if (!strcmp(entry->key, key)) return NULL;
    }
    if (!entry) bucket->first = new;
    else if (!strcmp(entry->key, key)) return NULL;
    else entry->next = new;
    return new;
}

Entry* insert(Scope* level, const char* key, void* value) {
    return append(get_bucket(level->buckets, key), key, value);
}

Entry* add(Stack* stack, const char* key, void* value) {
    return insert(stack->top, key, value);
}

Entry* find_entry(Stack* stack, const char* key) {
    for (Scope* level = stack->top; level; level = level->next) {
        Bucket* bucket = get_bucket(level->buckets, key);
        for (Entry* entry = bucket->first; entry; entry = entry->next) {
            if (!strcmp(entry->key, key)) return entry;
        }
    }
    return NULL;
}

void* find(Stack* stack, const char* key) {
    Entry* entry = find_entry(stack, key);
    if (entry) return entry->value;
    else return NULL;
}

Scope* push(Stack* stack) {
    Scope* new = make_scope(stack->top);
    stack->top = new;
    if (!stack->base) {
        stack->base = stack->top;
    }
    return new;
}

Scope* pop(Stack* stack) {
    Scope* popped = stack->top;
    stack->top = stack->top->next;
    return popped;
}

void print_stack(Stack* stack) {
    for (Scope* scope = stack->top; scope; scope = scope->next) {
        for (int i = 0; i < NUMBER_OF_BUCKETS; i++) {
            printf("| ");
            for (Entry* entry = scope->buckets[i].first; entry; entry = entry->next) {
                printf("%s:", entry->key);
                print_data(entry->value);
                printf(" ");
            }
        }
        printf("|\n");
    }
}
