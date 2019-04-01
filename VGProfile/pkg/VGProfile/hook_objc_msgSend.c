//
//  hook_objc_msgSend.c
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#include "hook_objc_msgSend.h"

#include "defines.h"

#ifdef SHOULE_HOOK_OBJC_MSG

#include <fishhook/fishhook.h>
#include <assert.h>

#pragma mark - hook_objc_msgSend -

__unused static void (*orig_objc_msgSend)(void);
static pre_objc_msg_send_func pre_objc_msgSend;
static post_objc_msg_send_func post_objc_msgSend;

__unused static inline void _pre_objc_msgSend(ID_TYPE self, SEL _cmd, uintptr_t link_register) {
    
    assert(pre_objc_msgSend != NULL);
    
    pre_objc_msgSend(self, _cmd, link_register);
}

__unused static inline uintptr_t _post_objc_msgSend() {
    assert(post_objc_msgSend != NULL);
    return post_objc_msgSend();
}

#define call(b, tmpr, value) \
__asm volatile ("stp x8, x9, [sp, #-16]!\n"); \
__asm volatile ("mov " #tmpr ", %0\n" :: "r"(value)); \
__asm volatile ("ldp x8, x9, [sp], #16\n"); \
__asm volatile (#b " " #tmpr "\n");

__attribute__((__naked__))
static void hook_objc_msgSend() {
    
    __asm
    volatile (
              "stp  x29, x30, [sp, #-16]!\n"
              "mov  x29, sp\n"
              
              // 调用subroutine前先备份调用后仍需使用的寄存器：
              //    - 临时寄存器
              //    - 函数输入参数
              //    `pre_objc_msgSend`等会被加载到`x8`等临时寄存器
              
              "stp  x8, x9, [sp, #-16]!\n"
              "stp  x0, x1, [sp, #-16]!\n"
              "stp  x2, x3, [sp, #-16]!\n"
              
              // 函数参数
              "mov  x2, lr\n"
              );
    
    call(blr, x10, &_pre_objc_msgSend)
    
    __asm
    volatile (
              // 恢复寄存器的值
              "ldp  x2, x3, [sp], #16\n"
              "ldp  x0, x1, [sp], #16\n"
              "ldp  x8, x9, [sp], #16\n"
              "ldp  x29, x30, [sp], #16\n"
              );
    
    call(blr, x10, orig_objc_msgSend)
    
    __asm
    volatile (
              // 备份`orig_objc_msgSend`的返回值
              "stp  x0,x1, [sp, #-16]!\n"
              );
    
    call(blr, x10, &_post_objc_msgSend)
    
    __asm
    volatile (
              "mov  lr, x0\n"
              // 取回`orig_objc_msgSend`的返回值
              "ldp  x0,x1, [sp], #16\n"
              
              "ret\n"
              );
}

void start_hook_objc_msgSend(pre_objc_msg_send_func pre, post_objc_msg_send_func post) {
    assert(pre != NULL);
    assert(post != NULL);
    
    pre_objc_msgSend = pre;
    post_objc_msgSend = post;
    
    rebind_symbols((struct rebinding[1]){
        {"objc_msgSend", (void*)hook_objc_msgSend, (void**)&orig_objc_msgSend},
    }, 1);
}

#else  // #if SHOULE_HOOK_OBJC_MSG

void start_hook_objc_msgSend() {}

#endif // #if SHOULE_HOOK_OBJC_MSG
