//
//  objc_msg_tracer.c
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#include "objc_msg_tracer.h"
#include "defines.h"
#include <stdio.h>

#ifdef SHOULE_HOOK_OBJC_MSG

#include <pthread.h>
#include <objc/runtime.h>
#include <dispatch/once.h>
#include <sys/time.h>
#include <assert.h>
#include "vg_stack.h"
#include "hook_objc_msgSend.h"
#include <sys/time.h>
typedef struct timeval timeval_t;

#define DEFAULT_STACK_SIZE 64
#define DEFAULT_MIN_TIME_COST ((timeval_t){0, 1000})

static timeval_t _min_time_cost = DEFAULT_MIN_TIME_COST;
static size_t _max_call_depth = DEFAULT_MAX_CALL_DEPTH;
static bool _call_record_enabled = true;
static vg_stack_t *_record_stack = NULL;
static pthread_key_t _hook_thread_key;


#pragma mark - records -
vg_stack_t *_get_record_stack() {
    if (_record_stack == NULL) {
        _record_stack = vg_stack_create(sizeof(vg_call_record_t), DEFAULT_STACK_SIZE);
    }
    assert(_record_stack != NULL);
    return _record_stack;
}
vg_call_record_t *_push_record() {
    vg_stack_t *stack = _get_record_stack();
    return vg_stack_push(stack);
}

void vg_clear_records() {
    vg_stack_release(&_record_stack);
}

vg_call_tree_t* vg_get_call_tree(size_t max_depth) {
    return vg_get_call_tree_from_records(_record_stack, max_depth);
}

#pragma mark - objc_call -
typedef struct {
    uintptr_t link_register; // 记录`hook_objc_msgSend`中`objc_msgSend`的返回地址
    //    ID_TYPE self;
    __unsafe_unretained Class clazz;
    SEL _cmd;
    timeval_t start_time;
} objc_call_t;

#pragma mark - hook_thread_data -
typedef struct {
    vg_stack_t stack;
    bool is_main_thread;
} hook_thread_data_t;

void hook_thread_data_release(void *ptr) {
    hook_thread_data_t *data = (hook_thread_data_t*)ptr;
    if (data == NULL) {
        return;
    }
    
    vg_stack_release_members(&data->stack);
    free(data);
}

hook_thread_data_t* hook_thread_data_create() {
    hook_thread_data_t *data = (hook_thread_data_t*)malloc(sizeof(hook_thread_data_t));
    
    vg_stack_init(&data->stack, sizeof(objc_call_t), DEFAULT_STACK_SIZE);
    data->is_main_thread = pthread_main_np();
    
    return data;
}

hook_thread_data_t* hook_thread_data_get() {
    hook_thread_data_t *data = (hook_thread_data_t*)pthread_getspecific(_hook_thread_key);
    if (data == NULL) {
        data = hook_thread_data_create();
        assert(data != NULL);
        
        pthread_setspecific(_hook_thread_key, data);
    }
    
    return data;
}

#pragma mark - hook_objc_msgSend -
static void push_objc_call(ID_TYPE self, SEL _cmd, uintptr_t link_register) {
    hook_thread_data_t *data = hook_thread_data_get();
    vg_stack_t *stack = &data->stack;
    
    objc_call_t *call = vg_stack_push(stack);
    call->_cmd = _cmd;
    call->clazz = object_getClass(self);
    call->link_register = link_register;
    
    if (data->is_main_thread && _call_record_enabled) {
        timeval_t now;
        gettimeofday(&now, NULL);
        call->start_time = now;
    }
}
static uintptr_t pop_objc_call() {
    hook_thread_data_t *data = hook_thread_data_get();
    vg_stack_t *stack = &data->stack;
    
    objc_call_t *call = vg_stack_top(stack);
    assert(call != NULL);
    uintptr_t link_register = call->link_register;
    if (data->is_main_thread && _call_record_enabled) {
        timeval_t cost;
        gettimeofday(&cost, NULL);
        timersub(&cost, &call->start_time, &cost);
        
        if (timercmp(&cost, &_min_time_cost, >=) && stack->length <= _max_call_depth) {
            vg_call_record_t *record = _push_record();
            record->clazz = call->clazz;
            record->cmd = call->_cmd;
            record->depth = stack->length - 1;
            record->cost = cost;
        }
    }
    
    vg_stack_pop(stack);
    return link_register;
}



void vg_start_tracer(void) {
    _call_record_enabled = true;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        pthread_key_create(&_hook_thread_key, hook_thread_data_release);
        start_hook_objc_msgSend(push_objc_call, pop_objc_call);
    });
}
void vg_stop_tracer(void) {
    _call_record_enabled = false;
}
void vg_set_min_time_cost(timeval_t cost){
    _min_time_cost = cost;
}
void vg_set_max_call_depth(size_t depth) {
    _max_call_depth = depth;
}

#else
void vg_start_tracer(void){}
void vg_stop_tracer(void){}
void vg_set_min_time_cost(timeval_t cost){}
void vg_set_max_call_depth(size_t depth){}

void vg_clear_records(void){}
vg_call_tree_t* vg_get_call_tree(size_t max_depth){return NULL;}

#endif // #ifdef SHOULE_HOOK_OBJC_MSG
