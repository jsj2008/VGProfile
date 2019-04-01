//
//  vg_call_tree.c
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#include "vg_call_tree.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

vg_call_tree_t* vg_call_tree_create(void) {
    vg_call_tree_t* tree = (vg_call_tree_t *)malloc(sizeof(vg_call_tree_t));
    vg_call_tree_init(tree);
    return tree;
}

void vg_call_tree_init(vg_call_tree_t *tree) {
    assert(tree != NULL);
    vg_tree_node_init_zero(&tree->root);
    tree->_record_buffer = NULL;
}

void vg_call_tree_release_members(vg_call_tree_t *tree) {
    assert(tree != NULL);
    vg_tree_node_init_zero(&tree->root);
    free(tree->_record_buffer);
    tree->_record_buffer = NULL;
}

void vg_call_tree_release(vg_call_tree_t **tree) {
    if (tree == NULL || *tree == NULL) {
        return;
    }
    
    vg_call_tree_release_members(*tree);
    free(*tree);
    *tree = NULL;
}

vg_call_tree_t *vg_get_call_tree_from_records(vg_stack_t *record_stack, size_t max_depth) {
    if (record_stack == NULL) {
        return NULL;
    }
    
    vg_call_tree_t *tree = vg_call_tree_create();
    vg_stack_copy_to_buffer(record_stack, &tree->_record_buffer);
    vg_call_record_t *records = tree->_record_buffer;
    
    size_t size = sizeof(vg_tree_node_t*) * max_depth;
    vg_tree_node_t **nodes = (vg_tree_node_t **)malloc(size);
    memset(nodes, 0, size);
    
    vg_tree_node_t *root = &tree->root;
    for (size_t idx = record_stack->length; idx > 0; --idx) {
        vg_call_record_t *record = &records[idx-1];
        
        if (record->depth >= 0 && record->depth < max_depth) {
            vg_tree_node_t *parent = record->depth > 0 ? nodes[record->depth-1] : root;
            if (parent != NULL) {
                vg_tree_node_t *current = vg_tree_node_create(record);
                vg_tree_insert(parent, current);
                nodes[record->depth] = current;
            }
        }
    }
    
    free(nodes);
    return tree;
}

double vg_call_record_cost_ms(vg_call_record_t* record) {
    return (double)record->cost.tv_sec * 1000 + (double)record->cost.tv_usec / 1000;
}
