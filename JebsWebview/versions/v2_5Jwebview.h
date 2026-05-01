#ifndef V2_5JWEBVIEW_H
#define V2_5JWEBVIEW_H

#define URL 300
#define DOCUMENT 900

typedef struct {
    int width;
    int height;
} JWindowSize;

typedef struct {
    JWindowSize WindowSize;
    int Zoom;

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

#endif