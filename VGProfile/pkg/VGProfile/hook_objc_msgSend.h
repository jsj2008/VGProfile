//
//  hook_objc_msgSend.h
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#ifndef hook_objc_msgSend_h
#define hook_objc_msgSend_h

#include <stdlib.h>
#import <objc/objc.h>

typedef __unsafe_unretained id ID_TYPE;

typedef void (*pre_objc_msg_send_func)(ID_TYPE self, SEL _cmd, uintptr_t link_register);
typedef uintptr_t (*post_objc_msg_send_func)(void);

void start_hook_objc_msgSend(pre_objc_msg_send_func pre, post_objc_msg_send_func post);

#endif /* hook_objc_msgSend_h */
