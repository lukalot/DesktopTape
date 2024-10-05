#import "AppLauncher.h"

@implementation AppLauncher

- (void)launchNewInstance {
    NSLog(@"launchNewInstance called");
    NSString *executablePath = [[NSBundle mainBundle] executablePath];
    NSLog(@"Executable path: %@", executablePath);
    NSTask *task = [[NSTask alloc] init];
    [task setLaunchPath:executablePath];
    [task launch];
    NSLog(@"New instance launched");
}

@end