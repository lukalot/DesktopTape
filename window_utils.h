#ifndef WINDOW_UTILS_H
#define WINDOW_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

void initializeCocoaApp();
void processCocoaEvents();
void runCocoaApp();
void setWindowLevelForAllSpaces(void* cocoaWindow);
void setWindowLevelBelowNormal(void* cocoaWindow);

#ifdef __cplusplus
}
#endif

#endif // WINDOW_UTILS_H