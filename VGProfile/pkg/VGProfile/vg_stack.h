//
//  vg_stack.h
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#ifndef vg_stack_h
#define vg_stack_h

#include <stdlib.h>
typedef struct {
    void* stack;    // `vg_stack_grow`时地址可能发生变化，
    //  外部不要长期引用`_stack`内的地址和
    //  `vg_stack_item_at`等的返回值
    //  可以通过`index`来记录栈中的元素
    //  或者使用`vg_page_stack_t`
    size_t length;  // `size_t`为无符号，做减法运算时要注意
    size_t capacity;
    size_t item_size;
} vg_stack_t;

vg_stack_t *vg_stack_create(size_t item_size, size_t capacity);
void vg_stack_release(vg_stack_t **stack);
void vg_stack_release_members(vg_stack_t *stack);
void vg_stack_init(vg_stack_t *stack, size_t item_size, size_t capacity);
void vg_stack_grow(vg_stack_t *stack, size_t cap);
void* vg_stack_top(vg_stack_t *stack);
void* vg_stack_item_at(vg_stack_t *stack, size_t index);
void* vg_stack_push(vg_stack_t *stack);
void vg_stack_pop(vg_stack_t *stack);
void vg_stack_copy(vg_stack_t *dst, vg_stack_t *src);
void vg_stack_copy_to_buffer(vg_stack_t *stack, void **buffer_ptr);


#endif /* vg_stack_h */
