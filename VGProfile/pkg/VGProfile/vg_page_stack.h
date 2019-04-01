//
//  vg_page_stack.h
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#ifndef vg_page_stack_h
#define vg_page_stack_h

#include <stdio.h>
#include "vg_stack.h"

/**
 * TODO: `vg_stack_t` grow时，stack指向的地址可能发生变化，所以
 *      外部不应该长期引用`stack`内的地址和`vg_stack_item_at`等的返回值
 * - grow不`realloc`存储数据的内存块，而是增加一个`page`
 *      `page`由一个`vg_stack_t`管理，便于随机访问
 * - 使用`page`的链表：不支持O(1)随机访问
 **/

//typedef struct {
//    vg_stack_t _inner;
//    size_t item_size;
//    size_t page_capacity;
//} vg_page_stack_t;

//void vg_page_stack_init(vg_stack_t *stack, size_t item_size, size_t page_capacity_log2, size_t stack_capacity);

#endif /* vg_page_stack_h */
