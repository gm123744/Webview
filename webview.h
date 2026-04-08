#ifndef JEB_H
#define JEB_H

#include <stdbool.h>
#include <unistd.h>  // for pid_t

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
    RELOADWINDOW = 0,
    BACK = 1,
    FORWARD = 2,
    CLOSETAB = 3
} JSignal;

typedef enum {
    ADDRESSBAR = 1,
    FULLSCREEN = 1,
    INCOGNITO = 1,
    KIOSKMODE = 1,
    NO_ADDRESSBAR = 0,
    NO_FULLSCREEN = 0,
    NO_INCOGNITO = 0,
    NO_KIOSKMODE = 0,
    NO_JEB_LOG = false
} JEflags;

void WindowSizeInit(JWindowSize *sz, int width, int height);
int WindowSettingsInit(JWindowSettings *settings, int addressbar, int fullscreen, int incognito, int kioskmode);
int DisplayContent(JDisplayContent *content, int Ctype, char* buffer);
void CreateContext(JWindowSettings *settings, JDisplayContent *content, bool ErrorS);

#ifdef __cplusplus
}
#endif

#endif