//
//  VGMsgTracerTests.m
//  VGProfileTests
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "vg_stack.h"
#import "vg_tree.h"
#include "vg_call_tree.h"

@interface VGMsgTracerTests : XCTestCase

@end

static void _visit(void *context, vg_tree_node_t *node) {
    void (^visit)(vg_tree_node_t *node) = (__bridge void (^)(vg_tree_node_t *))(context);
    visit(node);
}

@implementation VGMsgTracerTests

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testStack1 {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
    
    
    vg_stack_t *stack = vg_stack_create(sizeof(int), 0);
    int* item0 = vg_stack_push(stack);
    *item0 = 10;
    int* item1 = vg_stack_push(stack);
    *item1 = 20;
    
    int top = *((int*)vg_stack_top(stack));
    XCTAssert(top, @"top值应该为20, 而非: %d", top);
    vg_stack_pop(stack);
    
    top = *((int*)vg_stack_top(stack));
    XCTAssert(top, @"top值应该为10, 而非: %d", top);
    vg_stack_release(&stack);
}

- (void)testStack2 {
    typedef struct {
        uint64_t x;
        uint64_t y;
    } record_t;
#define count 4
    record_t test[count] = {
        {10, 20},
        {32, 40},
        {111, 222},
        {4444, 1},
    };
    
    vg_stack_t *stack = vg_stack_create(sizeof(record_t), 0);
    
    
    for (NSInteger i = 0; i < count; i++) {
        record_t *record = vg_stack_push(stack);
        *record = test[i];
        XCTAssert(stack->length == i+1, @"stack 长度应该为%ld，而非%zu", i+1, stack->length);
    }
    
    for (NSInteger i = count-1; i >= 0; i--) {
        record_t *record = vg_stack_top(stack);
        XCTAssert(record->x == test[i].x && record->y == test[i].y, @"数据不相等，应为{%llu,%llu}，而非{%llu,%llu}", test[i].x, test[i].y, record->x, record->y);
        XCTAssert(stack->length == i+1,  @"stack 长度应该为%ld，而非%zu", i+1, stack->length);
        vg_stack_pop(stack);
    }
    
    vg_stack_release_members(stack);
    XCTAssert(stack->length == 0 && stack->capacity == 0 && stack->stack == NULL, @"清理成员不干净,len:%zu,cap:%zu,stack:%p", stack->length, stack->capacity, stack->stack);
    
    vg_stack_release(&stack);
#undef count
}

- (void)testStack3 {
#define LEN 5
    uint64_t values[LEN];
    vg_stack_t *stack = vg_stack_create(sizeof(uint64_t), 0);
    for (int i = 0; i < LEN; i++) {
        values[i] = 1000 + i;
        uint64_t *item = (uint64_t*)vg_stack_push(stack);
        *item = values[i];
    }
    
    for (int i = 0; i < LEN; i++) {
        uint64_t *item = (uint64_t*)vg_stack_item_at(stack, i);
        XCTAssert(*item == values[i], @"*item 应为%llu, 实为:%llu", values[i], *item);
    }
#undef LEN
}

- (NSString *)formatedValues:(uint64_t *)values count:(int)count {
    NSMutableString *format = [NSMutableString string];
    
    for (int i = 0; i < count; i++) {
        [format appendFormat:@"_%llu", values[i]];
    }
    
    return format;
}
- (NSString *)formatedSizes:(int *)values count:(int)count {
    NSMutableString *format = [NSMutableString string];
    
    for (int i = 0; i < count; i++) {
        [format appendFormat:@"_%d", values[i]];
    }
    
    return format;
}


- (void)checkChildrenOfTree:(vg_tree_node_t *)tree values:(uint64_t *)values count:(int)count {
    NSString *expected = [self formatedValues:values count:count];
    int i = 0;
    NSMutableString *output = [NSMutableString string];
    
    void (^iter)(int i, vg_tree_node_t *child) = ^(int i, vg_tree_node_t *child){
        XCTAssert(i < count, @"迭代次数不应该超过%d", count);
        
        [output appendFormat:@"_%llu", *((uint64_t*)child->data)];
    };
    
    vg_tree_for_each_child(tree, child, {
        iter(i, child);
        i++;
    });
    
    XCTAssert([expected isEqualToString:output], @"应为:%@,实为:%@", expected, output);
    
    
    XCTAssert(i == count, @"迭代次数应该为%d,实为:%d", count, i);
}
- (void)checkChildrenOfTree_iter:(vg_tree_node_t *)tree values:(uint64_t *)values count:(int)count {
    NSString *expected = [self formatedValues:values count:count];
    int i = 0;
    NSMutableString *output = [NSMutableString string];
    
    
    int *i_ptr = &i;
    void (^iter)(vg_tree_node_t *) = ^(vg_tree_node_t *child) {
        XCTAssert(*i_ptr < count, @"迭代次数不应该超过%d", count);
        
        [output appendFormat:@"_%llu", *((uint64_t*)child->data)];
        (*i_ptr)++;
    };
    
    vg_tree_for_each_child_iter(tree, (__bridge void *)(iter), &_visit);
    
    XCTAssert([expected isEqualToString:output], @"应为:%@,实为:%@", expected, output);
    
    XCTAssert(i == count, @"迭代次数应该为%d,实为:%d", count, i);
}
- (void)checkTreeTraversal:(vg_tree_node_t *)tree values:(uint64_t *)values count:(int)count {
    int i = 0;
    NSString *expected = [self formatedValues:values count:count];
    NSMutableString *output = [NSMutableString string];
    
    void (^iter)(int i, vg_tree_node_t *child) = ^(int i, vg_tree_node_t *child){
        XCTAssert(i < count, @"迭代次数不应该超过%d", count);
        [output appendFormat:@"_%llu", *((uint64_t*)child->data)];
    };
    
    
    vg_tree_preorder_traversal(tree, child, 64, {
        iter(i, child);
        i++;
    });
    
    XCTAssert([expected isEqualToString:output], @"应为:%@,实为:%@", expected, output);
    XCTAssert(i == count, @"迭代次数应该为%d,实为:%d", count, i);
}

- (void)checkTreeTraversal_iter:(vg_tree_node_t *)tree values:(uint64_t *)values count:(int)count {
    int i = 0;
    NSString *expected = [self formatedValues:values count:count];
    NSMutableString *output = [NSMutableString string];
    int *i_ptr = &i;
    
    void (^visit)(vg_tree_node_t *node) = ^(vg_tree_node_t *node) {
        XCTAssert(*i_ptr < count, @"迭代次数不应该超过%d", count);
        [output appendFormat:@"_%llu", *((uint64_t*)node->data)];
        (*i_ptr)++;
    };
    
    vg_tree_preorder_traversal_iter(tree, (__bridge void *)(visit), &_visit);
    
    XCTAssert([expected isEqualToString:output], @"应为:%@,实为:%@", expected, output);
    XCTAssert(i == count, @"迭代次数应该为%d,实为:%d", count, i);
}

- (void)testTree1 {
#define LEN 5
    uint64_t dataset[LEN] = {0,1,2,3,4};
    vg_tree_node_t nodes[LEN];
    
    for (int i = 0; i < LEN; i++) {
        vg_tree_node_init(&nodes[i], &dataset[i]);
        XCTAssert(*((uint64_t*)nodes[i].data) == dataset[i], @"nodes[i].data应为:%llu, 实非:%llu", dataset[i], *((uint64_t*)nodes[i].data));
    }
    
    vg_tree_node_t *root = &nodes[0];
    vg_tree_insert(root, &nodes[4]);
    vg_tree_insert(root, &nodes[3]);
    vg_tree_insert(root, &nodes[2]);
    vg_tree_insert(root, &nodes[1]);
    
    uint64_t iter_data[] = {1,2,3,4};
    [self checkChildrenOfTree:root values:iter_data count:4];
#undef LEN
}
- (void)testTree2 {
#define LEN 5
    uint64_t dataset[LEN] = {0,1,2,3,4};
    vg_tree_node_t nodes[LEN];
    
    for (int i = 0; i < LEN; i++) {
        vg_tree_node_init(&nodes[i], &dataset[i]);
        XCTAssert(*((uint64_t*)nodes[i].data) == dataset[i], @"nodes[i].data应为:%llu, 实非:%llu", dataset[i], *((uint64_t*)nodes[i].data));
    }
    
    vg_tree_node_t *root = &nodes[0];
    vg_tree_insert(root, &nodes[1]);
    vg_tree_insert(root, &nodes[4]);
    
    nodes[3].sibling = &nodes[2];
    vg_tree_insert(root, &nodes[3]);
    
    uint64_t iter_data[] = {3, 2, 4, 1};
    [self checkChildrenOfTree:root values:iter_data count:4];
#undef LEN
}

- (void)testTree3 {
#define LEN 5
    uint64_t dataset[LEN] = {0,1,2,3,4};
    vg_tree_node_t nodes[LEN];
    
    for (int i = 0; i < LEN; i++) {
        vg_tree_node_init(&nodes[i], &dataset[i]);
        XCTAssert(*((uint64_t*)nodes[i].data) == dataset[i], @"nodes[i].data应为:%llu, 实非:%llu", dataset[i], *((uint64_t*)nodes[i].data));
    }
    
    vg_tree_node_t *root = &nodes[0];
    vg_tree_insert(root, &nodes[1]);
    vg_tree_insert(root, &nodes[2]);
    vg_tree_append(root, &nodes[3]);
    vg_tree_append(root, &nodes[4]);
    
    
    uint64_t iter_data[] = {2, 1, 3, 4};
    [self checkChildrenOfTree:root values:iter_data count:4];
#undef LEN
}

- (void)testTree4 {
#define LEN 10
    uint64_t dataset[LEN] = {};
    for (int i = 0; i < LEN; i++) {
        dataset[i] = i;
    }
    
    vg_tree_node_t nodes[LEN];
    
    for (int i = 0; i < LEN; i++) {
        vg_tree_node_init(&nodes[i], &dataset[i]);
        XCTAssert(*((uint64_t*)nodes[i].data) == dataset[i], @"nodes[i].data应为:%llu, 实非:%llu", dataset[i], *((uint64_t*)nodes[i].data));
    }
    
    vg_tree_node_t *root = &nodes[0];
    vg_tree_insert(root, &nodes[3]);
    vg_tree_insert(root, &nodes[2]);
    vg_tree_insert(root, &nodes[1]);
    vg_tree_insert(&nodes[1], &nodes[5]);
    vg_tree_insert(&nodes[1], &nodes[4]);
    vg_tree_insert(&nodes[5], &nodes[6]);
    vg_tree_insert(&nodes[2], &nodes[7]);
    vg_tree_insert(&nodes[7], &nodes[9]);
    vg_tree_insert(&nodes[7], &nodes[8]);
    
    [self checkChildrenOfTree:root values:(uint64_t[]){1,2,3} count:3];
    [self checkChildrenOfTree:&nodes[1] values:(uint64_t[]){4, 5} count:2];
    [self checkChildrenOfTree:&nodes[5] values:(uint64_t[]){6} count:1];
    [self checkChildrenOfTree:&nodes[2] values:(uint64_t[]){7} count:1];
    [self checkChildrenOfTree:&nodes[7] values:(uint64_t[]){8, 9} count:2];
    
    [self checkTreeTraversal:root values:(uint64_t[]){0, 1, 4, 5, 6, 2, 7, 8, 9, 3} count:LEN];
#undef LEN
}

- (void)testTree5 {
#define LEN 10
    uint64_t dataset[LEN] = {};
    for (int i = 0; i < LEN; i++) {
        dataset[i] = i;
    }
    
    vg_tree_node_t nodes[LEN];
    
    for (int i = 0; i < LEN; i++) {
        vg_tree_node_init(&nodes[i], &dataset[i]);
        XCTAssert(*((uint64_t*)nodes[i].data) == dataset[i], @"nodes[i].data应为:%llu, 实非:%llu", dataset[i], *((uint64_t*)nodes[i].data));
    }
    
    vg_tree_node_t *root = &nodes[0];
    vg_tree_insert(root, &nodes[3]);
    vg_tree_insert(root, &nodes[2]);
    vg_tree_insert(root, &nodes[1]);
    vg_tree_insert(&nodes[1], &nodes[5]);
    vg_tree_insert(&nodes[1], &nodes[4]);
    vg_tree_insert(&nodes[5], &nodes[6]);
    vg_tree_insert(&nodes[2], &nodes[7]);
    vg_tree_insert(&nodes[7], &nodes[9]);
    vg_tree_insert(&nodes[7], &nodes[8]);
    
    [self checkChildrenOfTree:root values:(uint64_t[]){1,2,3} count:3];
    [self checkChildrenOfTree:&nodes[1] values:(uint64_t[]){4, 5} count:2];
    [self checkChildrenOfTree:&nodes[5] values:(uint64_t[]){6} count:1];
    [self checkChildrenOfTree:&nodes[2] values:(uint64_t[]){7} count:1];
    [self checkChildrenOfTree:&nodes[7] values:(uint64_t[]){8, 9} count:2];
    
    [self checkTreeTraversal_iter:root values:(uint64_t[]){0, 1, 4, 5, 6, 2, 7, 8, 9, 3} count:LEN];
#undef LEN
}

- (void)testTree6 {
#define LEN 5
    uint64_t dataset[LEN] = {0,1,2,3,4};
    vg_tree_node_t nodes[LEN];
    
    for (int i = 0; i < LEN; i++) {
        vg_tree_node_init(&nodes[i], &dataset[i]);
        XCTAssert(*((uint64_t*)nodes[i].data) == dataset[i], @"nodes[i].data应为:%llu, 实非:%llu", dataset[i], *((uint64_t*)nodes[i].data));
    }
    
    vg_tree_node_t *root = &nodes[0];
    vg_tree_insert(root, &nodes[4]);
    vg_tree_insert(root, &nodes[3]);
    vg_tree_insert(root, &nodes[2]);
    vg_tree_insert(root, &nodes[1]);
    
    [self checkChildrenOfTree_iter:root values:(uint64_t[]){1,2,3,4} count:4];
#undef LEN
}

- (void)testCallTree1 {
#define LEN 12
    vg_stack_t record_stack;
    
    vg_stack_init(&record_stack, sizeof(vg_call_record_t), 0);
    int depths[LEN] = {3, 2, 4, 3, 2, 3, 2, 1, 3, 2, 1, 0};
    int want_depths[LEN] = {0, 1, 2, 3, 2, 3, 4, 2, 3, 1, 2, 3};
    
    for (int i = 0; i < LEN; i++) {
        vg_call_record_t *record = vg_stack_push(&record_stack);
        record->depth = depths[i];
    }
    XCTAssert(record_stack.length == LEN, @"record_stack.length应为:%d, 而非:%zu", LEN, record_stack.length);
    
    vg_call_tree_t *tree = vg_get_call_tree_from_records(&record_stack, 5);
    
    NSString *expected = [self formatedSizes:want_depths count:LEN];
    NSMutableString *got = [NSMutableString string];
    
    void (^visit)(vg_tree_node_t *node) = ^(vg_tree_node_t *node) {
        vg_call_record_t *record = node->data;
        if (record == NULL) {
            return;
        }
        [got appendFormat:@"_%zu", record->depth];
    };
    
    vg_tree_preorder_traversal_iter(&tree->root, (__bridge void *)(visit), _visit);
    
    XCTAssert([expected isEqualToString:got], @"应为:%@,实为:%@", expected, got);
    
    vg_call_tree_release(&tree);
    vg_stack_release_members(&record_stack);
    
    
#undef LEN
}


- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
