//
//  VGMsgTracer.m
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#import "VGMsgTracer.h"
#import "objc_msg_tracer.h"
#import <objc/runtime.h>

static void _visit(void *context, vg_tree_node_t *node) {
    void (^visit)(vg_tree_node_t *node) = (__bridge void (^)(vg_tree_node_t *))(context);
    visit(node);
}

@implementation VGMsgTracer
+ (void)start {
    vg_start_tracer();
}

+ (void)startWithMaxDepth:(size_t)depth {
    vg_set_max_call_depth(depth);
    vg_start_tracer();
}

+ (void)startWithMinCost:(timeval_t)cost {
    vg_set_min_time_cost(cost);
    vg_start_tracer();
}

+ (void)startWithMaxDepth:(size_t)depth
                  minCost:(timeval_t)cost {
    vg_set_max_call_depth(depth);
    vg_set_min_time_cost(cost);
    vg_start_tracer();
}

+ (void)printCallTree {
    vg_call_tree_t *tree = vg_get_call_tree(DEFAULT_MAX_CALL_DEPTH);
    
    NSMutableString *text = [NSMutableString string];
    
    void (^visit)(vg_tree_node_t *node) = ^(vg_tree_node_t *node){
        vg_call_record_t *record = (vg_call_record_t *)node->data;
        if (record == NULL) {
            return ;
        }
        
        NSString *clazz = NSStringFromClass(record->clazz);
        NSString *method = NSStringFromSelector(record->cmd);
        
        NSString *padding = [@"" stringByPaddingToLength:record->depth * 4 withString:@" " startingAtIndex:0];
        [text appendFormat:@"%@[%@ %@] %gms\n", padding, clazz, method, vg_call_record_cost_ms(record)];
    };
    
    vg_tree_preorder_traversal_iter(&tree->root, (__bridge void *)(visit), _visit);
    vg_call_tree_release(&tree);
    
    NSLog(@"\n%@", text);
}

+ (void)stop {
    vg_stop_tracer();
}

+ (void)cleanup {
    vg_stop_tracer();
    vg_clear_records();
}

@end
