//
//  vg_tree.h
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#ifndef vg_tree_h
#define vg_tree_h

#include <stdlib.h>

typedef struct _vg_tree_node {
    void* data;
    struct _vg_tree_node *child;
    struct _vg_tree_node *sibling;
} vg_tree_node_t;

void vg_tree_node_init(vg_tree_node_t *node, void* data);
void vg_tree_node_init_zero(vg_tree_node_t *node);
vg_tree_node_t * vg_tree_node_create(void* data);
void vg_tree_node_release(vg_tree_node_t **node_ptr, void(*release_data)(void*data));

void vg_tree_insert(vg_tree_node_t *parent, vg_tree_node_t *child);
void vg_tree_append(vg_tree_node_t *parent, vg_tree_node_t *child);

void vg_tree_for_each_child_iter(vg_tree_node_t *parent, void* context, void(*iter)(void* context, vg_tree_node_t *child));

#define vg_tree_for_each_child(parent, child, iter) \
{\
    vg_tree_node_t *child = parent != NULL ? parent->child : NULL;\
    while (child != NULL) {\
        iter\
        child = child->sibling;\
    }\
}

void vg_tree_preorder_traversal_iter(vg_tree_node_t *root, void* context, void(*visit)(void* context, vg_tree_node_t *node));

// root, node, capacity, visit
// `root`, `node`不要使用`_stack,_step,_child,_sibling`
#define vg_tree_preorder_traversal(root, node, capacity, visit)\
do {\
    if (root == NULL) {\
        break;\
    }\
    \
    vg_stack_t _stack;\
    vg_stack_t _steps;\
    vg_stack_init(&_stack, sizeof(vg_tree_node_t*), capacity);\
    vg_stack_init(&_steps, sizeof(uint8_t*), capacity);\
    \
    *(vg_tree_node_t **)vg_stack_push(&_stack) = root;\
    *(uint8_t*)vg_stack_push(&_steps) = 0;\
    \
    while (_stack.length > 0 && _steps.length > 0) {\
        vg_tree_node_t *node = *(vg_tree_node_t **)vg_stack_top(&_stack);\
        uint8_t *step = (uint8_t *)vg_stack_top(&_steps);\
        switch (*step) {\
            case 0: {\
                visit\
                if (node->child != NULL) {\
                    *(vg_tree_node_t **)vg_stack_push(&_stack) = node->child;\
                    *(uint8_t*)vg_stack_push(&_steps) = 0;\
                    *step = 1;\
                } else if (node->sibling != NULL) {\
                    *(vg_tree_node_t **)vg_stack_push(&_stack) = node->sibling;\
                    *(uint8_t*)vg_stack_push(&_steps) = 0;\
                    *step = 2;\
                } else {\
                    vg_stack_pop(&_stack);\
                    vg_stack_pop(&_steps);\
                }\
                break;\
            }\
            case 1: {\
                if (node->sibling != NULL) {\
                    *(vg_tree_node_t **)vg_stack_push(&_stack) = node->sibling;\
                    *(uint8_t*)vg_stack_push(&_steps) = 0;\
                    *step = 2;\
                } else {\
                    vg_stack_pop(&_stack);\
                    vg_stack_pop(&_steps);\
                }\
                break;\
            }\
            default: {\
                vg_stack_pop(&_stack);\
                vg_stack_pop(&_steps);\
                break;\
            }\
        }\
    }\
    vg_stack_release_members(&_stack);\
    vg_stack_release_members(&_steps);\
} while (0);



#endif /* vg_tree_h */
