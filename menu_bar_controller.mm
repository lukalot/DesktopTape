#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import "AppLauncher.h"
#import "menu_bar_controller.h"

@interface MenuBarController : NSObject
- (void)setupMenuBar;
@end

@implementation MenuBarController

+ (void)initialize {
    if (self == [MenuBarController class]) {
        [[NSUserDefaults standardUserDefaults] registerDefaults:@{
            @"NSApplicationCrashOnExceptions": @YES
        }];
    }
}

- (void)setupMenuBar {
    NSApplication *application = [NSApplication sharedApplication];
    NSMenu *mainMenu = [[NSMenu alloc] init];
    [application setMainMenu:mainMenu];
    
    // Application menu
    NSMenuItem *appMenuItem = [[NSMenuItem alloc] init];
    NSMenu *appMenu = [[NSMenu alloc] init];
    [appMenuItem setSubmenu:appMenu];
    [mainMenu addItem:appMenuItem];
    
    [appMenu addItemWithTitle:@"Quit Desktop Tape" action:@selector(terminate:) keyEquivalent:@"q"];
    
    // Edit menu
    NSMenuItem *editMenuItem = [[NSMenuItem alloc] initWithTitle:@"Edit" action:nil keyEquivalent:@""];
    NSMenu *editMenu = [[NSMenu alloc] initWithTitle:@"Edit"];
    [editMenuItem setSubmenu:editMenu];
    [editMenu addItemWithTitle:@"Undo" action:@selector(undo:) keyEquivalent:@"z"];
    [editMenu addItemWithTitle:@"Redo" action:@selector(redo:) keyEquivalent:@"Z"];
    [editMenu addItem:[NSMenuItem separatorItem]];
    [editMenu addItemWithTitle:@"Cut" action:@selector(cut:) keyEquivalent:@"x"];
    [editMenu addItemWithTitle:@"Copy" action:@selector(copy:) keyEquivalent:@"c"];
    [editMenu addItemWithTitle:@"Paste" action:@selector(paste:) keyEquivalent:@"v"];
    [editMenu addItemWithTitle:@"Delete" action:@selector(delete:) keyEquivalent:@"\b"];
    [editMenu addItem:[NSMenuItem separatorItem]];
    [editMenu addItemWithTitle:@"Select All" action:@selector(selectAll:) keyEquivalent:@"a"];
    [mainMenu addItem:editMenuItem];
    
    // Tape menu
    NSMenuItem *tapeMenuItem = [[NSMenuItem alloc] initWithTitle:@"Tape" action:nil keyEquivalent:@""];
    NSMenu *tapeMenu = [[NSMenu alloc] initWithTitle:@"Tape"];
    [tapeMenuItem setSubmenu:tapeMenu];
    [tapeMenu addItemWithTitle:@"Increase Font Size" action:@selector(increaseFontSize:) keyEquivalent:@"+"];
    [tapeMenu addItemWithTitle:@"Decrease Font Size" action:@selector(decreaseFontSize:) keyEquivalent:@"-"];
    [mainMenu addItem:tapeMenuItem];
    
    // Modify the new instance menu item setup
    NSMenuItem *newInstanceItem = [[NSMenuItem alloc] initWithTitle:@"New Instance" 
                                                            action:@selector(launchNewInstance:) 
                                                     keyEquivalent:@"n"];
    [newInstanceItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand];
    [newInstanceItem setTarget:NSApp]; // Set target to NSApp
    [mainMenu addItem:newInstanceItem];
}

- (void)showAbout:(id)sender {
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:@"About Desktop Tape"];
    [alert setInformativeText:@"Version 1.0\n© 2023 Your Company"];
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}

- (void)increaseFontSize:(id)sender {
    NSLog(@"Increase Font Size action");
    // Implement font size increase logic here
}

- (void)decreaseFontSize:(id)sender {
    NSLog(@"Decrease Font Size action");
    // Implement font size decrease logic here
}

- (void)resetFontSize:(id)sender {
    NSLog(@"Reset Font Size action");
    // Implement font size reset logic here
}

- (void)toggleTransparency:(id)sender {
    NSLog(@"Toggle Transparency action");
    // Implement transparency toggle logic here
}

- (void)toggleAlwaysOnTop:(id)sender {
    NSLog(@"Toggle Always on Top action");
    // Implement always-on-top toggle logic here
}

- (void)launchNewInstance {
    AppLauncher *launcher = [[AppLauncher alloc] init];
    [launcher launchNewInstance];
}

@end

@implementation NSApplication (NewInstanceLauncher)

- (void)launchNewInstance:(id)sender {
    AppLauncher *launcher = [[AppLauncher alloc] init];
    [launcher launchNewInstance];
}

@end

// C++ wrapper function
extern "C" void setupMenuBar() {
    @autoreleasepool {
        MenuBarController *controller = [[MenuBarController alloc] init];
        [controller setupMenuBar];
    }
}