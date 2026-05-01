#ifndef V2_0JWEBVIEW_H
#define V2_0JWEBVIEW_H


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

int CreateContext(JWindowSettings *settings, JDisplayContent *content, int ChromiumLogs, bool ErrorS);
void DestroyContext(int pid, int signal);
void JwebviewTerminate(int pid);

#endif