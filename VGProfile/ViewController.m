//
//  ViewController.m
//  VGProfile
//
//  Created by zhangxq on 2019/4/1.
//  Copyright © 2019 vargeek. All rights reserved.
//

#import "ViewController.h"
#import "VGMsgTracer.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    UIButton *button = [UIButton buttonWithType:UIButtonTypeInfoDark];
    [self.view addSubview:button];
    button.frame = CGRectMake(50, 50, 100, 50);
    [button addTarget:self action:@selector(onClickButton:) forControlEvents:UIControlEventTouchUpInside];
    
    
    UIButton *clearBtn = [UIButton buttonWithType:UIButtonTypeInfoLight];
    [self.view addSubview:clearBtn];
    clearBtn.frame = CGRectMake(200, 50, 100, 50);
    [clearBtn addTarget:self action:@selector(onClickClearBtn:) forControlEvents:UIControlEventTouchUpInside];
    
}


- (void)onClickButton:(UIButton *) sender {
    [VGMsgTracer printCallTree];
}

- (void)onClickClearBtn:(UIButton *)sender {
    [VGMsgTracer cleanup];
    [VGMsgTracer start];
}
@end
