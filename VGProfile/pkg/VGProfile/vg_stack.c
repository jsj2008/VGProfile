//
//  vg_stack.c
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#include "vg_stack.h"
#include <assert.h>
#include <string.h>

#ifndef DOUBLE_CAPACITY_THRESHOLD
#define DOUBLE_CAPACITY_THRESHOLD 1024
#endif

#pragma mark - objc_call_stack -

vg_stack_t *vg_stack_create(size_t item_size, size_t capacity) {
    vg_stack_t *stack = (vg_stack_t*) malloc(sizeof(vg_stack_t));
    vg_stack_init(stack, item_size, capacity);
    return stack;
}
void vg_stack_release(vg_stack_t **stack) {
    if (stack == NULL || *stack == NULL) {
        return;
    }
    vg_stack_release_members(*stack);
    
    free(*stack);
    *stack = NULL;
}

void vg_stack_release_members(vg_stack_t *stack) {
    assert(stack != NULL);
    if (stack->stack != NULL) {
        free(stack->stack);
        stack->stack = NULL;
    }
    stack->capacity = 0;
    stack->length = 0;
}

void vg_stack_init(vg_stack_t *stack, size_t item_size, size_t capacity) {
    assert(stack != NULL);
    assert(item_size >= 0);
    assert(capacity >= 0);
    
    stack->item_size = item_size;
    stack->length = 0;
    stack->capacity = capacity;
    stack->stack = malloc(item_size * capacity);
}

void vg_stack_grow(vg_stack_t *stack, size_t cap) {
    assert(stack != NULL);
    
    int64_t old_cap = stack->capacity;
    int64_t double_old_cap = old_cap + old_cap;
    int64_t new_cap = old_cap;
    if (cap > double_old_cap) {
        new_cap = cap;
    } else {
        if (old_cap < DOUBLE_CAPACITY_THRESHOLD) {
            new_cap = double_old_cap;
        } else {
            // Check 0 < newcap to detect overflow
            // and prevent an infinite loop.
            while (new_cap > 0 && new_cap < cap) {
                new_cap += new_cap >> 2;
            }
            if (new_cap <= 0) {
                new_cap = cap;
            }
        }
    }
    
    stack->stack = realloc(stack->stack, stack->item_size * new_cap);
    assert(stack->stack != NULL);
    stack->capacity = new_cap;
}

#define _vg_stack_at(stack, index) (void*)&((uint8_t*)stack->stack)[(index)*stack->item_size]
#define _vg_stack_top(stack) _vg_stack_at(stack, stack->length-1)

void* vg_stack_top(vg_stack_t *stack) {
    assert(stack != NULL);
    return (stack->length > 0) ? _vg_stack_top(stack) : NULL;
}

void* vg_stack_item_at(vg_stack_t *stack, size_t index) {
    assert(stack != NULL);
    return _vg_stack_at(stack, index);
}

void* vg_stack_push(vg_stack_t *stack) {
    assert(stack != NULL);
    
    if (stack->length >= stack->capacity) {
        vg_stack_grow(stack, stack->length + 1);
    }
    assert(stack->length < stack->capacity);
    ++stack->length;
    return _vg_stack_top(stack);
}

void vg_stack_pop(vg_stack_t *stack) {
    assert(stack != NULL);
    if (stack->length > 0) {
        --stack->length;
    }
}


void vg_stack_copy(vg_stack_t *dst, vg_stack_t *src) {
    if (dst == NULL || src == NULL) {
        return;
    }
    
    if (dst->capacity > 0) {
        vg_stack_release_members(dst);
    }
    if (src->length > 0) {
        vg_stack_init(dst, src->item_size, src->length);
        memcpy(dst->stack, src->stack, src->item_size * src->length);
        dst->length = src->length;
    }
}

void vg_stack_copy_to_buffer(vg_stack_t *stack, void **buffer_ptr) {
    if (stack == NULL || buffer_ptr == NULL) {
        return;
    }
    
    free(*buffer_ptr);
    size_t size = stack->item_size * stack->length;
    *buffer_ptr = malloc(size);
    memcpy(*buffer_ptr, stack->stack, size);
}
