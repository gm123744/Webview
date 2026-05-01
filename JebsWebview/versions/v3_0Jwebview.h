#ifndef V3_0JWEBVIEW_H
#define V3_0JWEBVIEW_H

#include <stdbool.h>

#define JVERSION "3.0"
#define URL 300
#define DOCUMENT 900

typedef struct {
    int width;
    int height;
} JWindowSize;

typedef struct {
    bool Zoom;
    bool AddressBar;
    bool FullScreen;
    bool Incognito;
    bool KioskMode;
    bool GPU;
    bool ChromiumLogs;
    bool JWLOG;
    bool DisableGoogleFeatures;
    bool ForceDarkMode;
    bool HideScrollBars;
    bool DisableDevTools;
    bool SandBoxed;
    bool StrictChecking;
} JWindowSettings;

typedef struct {
    int Ctype;
    char buffer[4096];
} JDisplayContent;

typedef struct {
    int pid;
    int height;
    int width;
    char app_arg[1024];
    char browser_path[4096];
    JWindowSize WindowSize;
    JWindowSettings WindowSettings;
    JDisplayContent DisplayContent;
}JWindow;

typedef enum {
    JSIGINT = 2,
    JSIGKILL = 9,
    JSIGTERM = 15
} JSysSignal;

JWindow* JCreateContext(JWindowSize* WindowSize,JWindowSettings* WindowSettings,JDisplayContent* DisplayContent);
void JDestroyContext(JWindow* win, int signal);
void JWebviewTerminate(JWindow* win);
void JDebug(JWindow* win);
#endif