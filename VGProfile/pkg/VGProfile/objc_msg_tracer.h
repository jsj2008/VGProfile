//
//  objc_msg_tracer.h
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#ifndef objc_msg_tracer_h
#define objc_msg_tracer_h

#include <stdlib.h>
#include "types.h"
#include "vg_stack.h"
#include "vg_tree.h"
#include "vg_call_tree.h"

#define DEFAULT_MAX_CALL_DEPTH 5

void vg_start_tracer(void);
void vg_stop_tracer(void);
void vg_set_min_time_cost(timeval_t cost);
void vg_set_max_call_depth(size_t depth);

void vg_clear_records(void);
vg_call_tree_t* vg_get_call_tree(size_t max_depth);

#endif /* objc_msg_tracer_h */
