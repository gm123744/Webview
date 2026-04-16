#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <stdbool.h>
#include <limits.h>

//====================VERSION 2.0====================//
#if defined(JVERSION_2_0)

#ifdef __cplusplus
extern "C" {
#endif

#define URL 300
#define DOCUMENT 900

typedef struct {
    int width;
    int height;
} JWindowSize;

typedef struct {
    char title[128];
    int AddressBar;
    int FullScreen;
    int Incognito;
    int KioskMode;
    int DisableGPU;
    int Zoom;
    JWindowSize WindowSize;
} JWindowSettings;

typedef struct {
    int Ctype;
    char buffer[2048];
} JDisplayContent;

typedef enum {
    JSIGINT = 2,
    JSIGKILL = 9,
    JSIGTERM = 15
} JSysSignal;

int CreateContext(JWindowSettings *settings, JDisplayContent *content, int ChromiumLogs, bool ErrorS);
void DestroyContext(int pid, int debug, int signal);
void JwebviewTerminate(int pid);

#ifdef __cplusplus
}
#endif

#endif

//==================== VERSION 2.5 ====================//
#if defined(JVERSION_2_5)

#ifdef __cplusplus
extern "C" {
#endif

#define URL 300
#define DOCUMENT 900

typedef struct {
    int width;
    int height;
} JWindowSize;

typedef struct {
    JWindowSize WindowSize;
    int Zoom;
    
    //flags
    unsigned int AddressBar;
    unsigned int FullScreen;
    unsigned int Incognito;
    unsigned int KioskMode;
    unsigned int GPU;
    unsigned int ChromiumLogs;
    unsigned int JWLOG;
    unsigned int DisableGoogleFeatures;
    unsigned int ForceDarkMode;
    unsigned int HideScrollBars;
    unsigned int DisableDevTools;

} JWindowSettings;

typedef struct {
    int Ctype;
    char buffer[4096];
} JDisplayContent;

typedef enum {
    RELOADWINDOW = 0,
    BACK = 1,
    FORWARD = 2,
    CLOSETAB = 3
} JSignal;

typedef enum {
    JSIGINT = 2,
    JSIGKILL = 9,
    JSIGTERM = 15
} JSysSignal;

int CreateContext(JWindowSettings *settings, JDisplayContent *content);
void DestroyContext(int pid, int signal);
void JwebviewTerminate(int pid);

#ifdef __cplusplus
}
#endif

#endif

#endif
