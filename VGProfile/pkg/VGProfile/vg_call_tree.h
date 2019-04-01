//
//  vg_call_tree.h
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#ifndef vg_call_tree_h
#define vg_call_tree_h

#include <stdlib.h>
#include "types.h"
#include "vg_tree.h"
#include "vg_stack.h"

typedef struct {
    __unsafe_unretained Class clazz;
    SEL cmd;
    timeval_t cost;
    size_t depth;
} vg_call_record_t;

typedef struct {
    vg_tree_node_t root;
    void *_record_buffer;
} vg_call_tree_t;

double vg_call_record_cost_ms(vg_call_record_t* record);

vg_call_tree_t* vg_call_tree_create(void);
void vg_call_tree_init(vg_call_tree_t *tree);
void vg_call_tree_release_members(vg_call_tree_t *tree);
void vg_call_tree_release(vg_call_tree_t **tree);
vg_call_tree_t *vg_get_call_tree_from_records(vg_stack_t *record_stack, size_t max_depth);



#endif /* vg_call_tree_h */
