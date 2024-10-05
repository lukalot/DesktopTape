#import <Cocoa/Cocoa.h>
#include <string>
#include <iostream>

extern "C" void launchNewInstance() {
    @autoreleasepool {
        NSString *executablePath = [[NSBundle mainBundle] executablePath];
        NSTask *task = [[NSTask alloc] init];
        [task setLaunchPath:executablePath];
        [task launch];
    }
}