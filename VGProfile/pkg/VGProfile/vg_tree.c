//
//  vg_tree.c
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#include "vg_tree.h"
#include "vg_stack.h"

void vg_tree_node_init(vg_tree_node_t *node, void* data) {
    if (node == NULL) {
        return;
    }
    node->data = data;
    node->child = NULL;
    node->sibling = NULL;
}

void vg_tree_node_init_zero(vg_tree_node_t *node) {
    vg_tree_node_init(node, NULL);
}

vg_tree_node_t * vg_tree_node_create(void* data) {
    vg_tree_node_t *node = (vg_tree_node_t *)malloc(sizeof(vg_tree_node_t));
    vg_tree_node_init(node, data);
    return node;
}

// TODO: 测试是否内存泄露
void vg_tree_node_release(vg_tree_node_t **node_ptr, void(*release_data)(void*data)) {
    
    vg_tree_node_t *node = node_ptr != NULL ? *node_ptr : NULL;
    if (node == NULL) {
        return;
    }
    
    // release data
    if (release_data) {
        release_data(node->data);
    }
    node->data = NULL;
    
    // release child and sibling
    vg_tree_node_release(&node->child, release_data);
    vg_tree_node_release(&node->sibling, release_data);
    
    // free self
    free(node);
    *node_ptr = NULL;
}

void vg_tree_insert(vg_tree_node_t *parent, vg_tree_node_t *child) {
    if (parent == NULL || child == NULL) {
        return;
    }
    
    vg_tree_node_t *sibling = child;
    while (sibling->sibling != NULL) {
        sibling = sibling->sibling;
    }
    sibling->sibling = parent->child;
    parent->child = child;
}

void vg_tree_append(vg_tree_node_t *parent, vg_tree_node_t *child) {
    if (parent == NULL || child == NULL) {
        return;
    }
    if (parent->child == NULL) {
        parent->child = child;
    } else {
        vg_tree_node_t *sibling = parent->child;
        while (sibling->sibling != NULL) {
            sibling = sibling->sibling;
        }
        sibling->sibling = child;
    }
}

void vg_tree_for_each_child_iter(vg_tree_node_t *parent, void* context, void(*iter)(void* context, vg_tree_node_t *child)) {
    if (parent == NULL || iter == NULL) {
        return;
    }
    
    vg_tree_node_t *child = parent != NULL ? parent->child : NULL;
    while (child != NULL) {
        iter(context, child);
        child = child->sibling;
    }
}

void vg_tree_preorder_traversal_iter(vg_tree_node_t *root, void* context, void(*visit)(void* context, vg_tree_node_t *node)) {
    if (root == NULL || visit == NULL) {
        return;
    }
    visit(context, root);
    vg_tree_preorder_traversal_iter(root->child, context, visit);
    vg_tree_preorder_traversal_iter(root->sibling, context, visit);
}

//void vg_tree_preorder_traversal_(vg_tree_node_t *root, size_t capacity) {
//    do {
//        if (root == NULL) {
//            break;
//        }
//
//        vg_stack_t _stack;
//        vg_stack_t _steps;
//        vg_stack_init(&_stack, sizeof(vg_tree_node_t*), capacity);
//        vg_stack_init(&_steps, sizeof(uint8_t*), capacity);
//
//        *(vg_tree_node_t **)vg_stack_push(&_stack) = root;
//        *(uint8_t*)vg_stack_push(&_steps) = 0;
//
//        while (_stack.length > 0 && _steps.length > 0) {
//            vg_tree_node_t *node = *(vg_tree_node_t **)vg_stack_top(&_stack);
//            uint8_t *step = (uint8_t *)vg_stack_top(&_steps);
//            switch (*step) {
//                case 0: {
//                    // visit
//                    if (node->child != NULL) {
//                        *(vg_tree_node_t **)vg_stack_push(&_stack) = node->child;
//                        *(uint8_t*)vg_stack_push(&_steps) = 0;
//                        *step = 1;
//                    } else if (node->sibling != NULL) {
//                        *(vg_tree_node_t **)vg_stack_push(&_stack) = node->sibling;
//                        *(uint8_t*)vg_stack_push(&_steps) = 0;
//                        *step = 2;
//                    } else {
//                        vg_stack_pop(&_stack);
//                        vg_stack_pop(&_steps);
//                    }
//                    break;
//                }
//                case 1: {
//                    if (node->sibling != NULL) {
//                        *(vg_tree_node_t **)vg_stack_push(&_stack) = node->sibling;
//                        *(uint8_t*)vg_stack_push(&_steps) = 0;
//                        *step = 2;
//                    } else {
//                        vg_stack_pop(&_stack);
//                        vg_stack_pop(&_steps);
//                    }
//                    break;
//                }
//                default: {
//                    vg_stack_pop(&_stack);
//                    vg_stack_pop(&_steps);
//                    break;
//                }
//            }
//        }
//        vg_stack_release_members(&_stack);
//        vg_stack_release_members(&_steps);
//    } while (0);
//
//}


