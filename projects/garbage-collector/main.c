#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#define STACK_MAX 256

#define INITIAL_GC_THRESHOLD 10

typedef enum {
    OBJ_INT,
    OBJ_PAIR
} ObjectType;

typedef struct sObject {
    ObjectType type;
    unsigned char marked;

    struct sObject *next;

    union {
        int value;

        struct {
            struct sObject *first;
            struct sObject *second;
        };
    };
} Object;

typedef struct {
    Object *stack[STACK_MAX];
    int stackSize;

    int numObjects;
    int maxObjects;

    Object *firstObject;
} VM;

void gc(VM *vm);

VM *newVM() {
    VM *vm = malloc(sizeof(VM));
    vm->stackSize = 0;

    vm->numObjects = 0;
    vm->maxObjects = INITIAL_GC_THRESHOLD;

    return vm;
}

void push(VM *vm, Object *value) {
    assert(vm->stackSize < STACK_MAX);
    vm->stack[vm->stackSize++] = value;
}

Object *pop(VM *vm) {
    assert(vm->stackSize > 0);
    return vm->stack[--vm->stackSize];
}

Object *newObject(VM *vm, ObjectType objectType) {
    if (vm->numObjects == vm->maxObjects) gc(vm);

    Object *object = malloc(sizeof(Object));
    object->type = objectType;
    object->marked = 0;

    object->next = vm->firstObject;
    vm->firstObject = object;

    vm->numObjects++;
    return object;
}


void pushInt(VM *vm, int value) {
    Object *object = newObject(vm, OBJ_INT);
    object->value = value;
    push(vm, object);
}

Object *pushPair(VM *vm) {
    Object *object = newObject(vm, OBJ_PAIR);
    object->second = pop(vm);
    object->first = pop(vm);
    push(vm, object);
    return object;
}

void mark(Object *object) {
    if (!object || object->marked) return;

    object->marked = 1;

    if (object->type == OBJ_PAIR) {
        mark(object->first);
        mark(object->second);
    }
}

void markAll(VM *vm) {
    for (size_t i = 0; i < vm->stackSize; i++) {
        mark(vm->stack[i]);
    }
}

void sweep(VM *vm) {
    Object **object = &vm->firstObject;
    while (*object) {
        if (!(*object)->marked) {
            Object *unreached = *object;
            *object = unreached->next;
            vm->numObjects--;
            free(unreached);
        } else {
            (*object)->marked = 0;
            object = &(*object)->next;
        }
    }
}

void gc(VM *vm) {
    int numObjects = vm->numObjects;

    markAll(vm);
    sweep(vm);

    vm->maxObjects = numObjects * 2;
}

int main() {
    VM *vm = newVM();

    pushInt(vm, 10);
    pushInt(vm, 30);
    pushPair(vm);
    pushInt(vm, 100);
    pushPair(vm);

    pushInt(vm, 1000);
    Object* obj = pop(vm);

    printf("%d\n", obj->value);
    gc(vm);
    printf("%d", obj->marked);

    return 0;
}
