#import <Cocoa/Cocoa.h>
#include "window_utils.h"

void initializeCocoaApp() {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular]; // Change this line
    [NSApp finishLaunching];
    [[NSApp mainMenu] setAutoenablesItems:YES];
    [NSApp activateIgnoringOtherApps:YES];
}

void processCocoaEvents() {
    NSEvent *event;
    while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                       untilDate:[NSDate distantPast]
                                          inMode:NSDefaultRunLoopMode
                                         dequeue:YES])) {
        [NSApp sendEvent:event];
    }
}

void runCocoaApp() {
    [NSApp run];
}

void setWindowLevelForAllSpaces(void* cocoaWindow) {
    NSWindow* window = (__bridge NSWindow*)cocoaWindow;
    [window setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces |
                                  NSWindowCollectionBehaviorStationary |
                                  NSWindowCollectionBehaviorIgnoresCycle];
}

void setWindowLevelBelowNormal(void* cocoaWindow) {
    NSWindow* window = (__bridge NSWindow*)cocoaWindow;
    [window setLevel:NSNormalWindowLevel - 1];
}

// The setupMenuBar function should be implemented in menu_bar_controller.mm