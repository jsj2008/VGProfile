//
//  VGMsgTracer.h
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "types.h"

NS_ASSUME_NONNULL_BEGIN

@interface VGMsgTracer : NSObject
+ (void)start;
+ (void)startWithMaxDepth:(size_t)depth;
+ (void)startWithMinCost:(timeval_t)cost;
+ (void)startWithMaxDepth:(size_t)depth
                  minCost:(timeval_t)cost;

+ (void)printCallTree;
+ (void)stop;
+ (void)cleanup;

@end

NS_ASSUME_NONNULL_END
