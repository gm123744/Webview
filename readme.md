#   Jeb Webview Tutorial

jebs Webview is a small launcher which uses the Chromium browser to render HTML files and URL files with minimal syntax.

---

## Webview Functions

```
//Version 2.0
int CreateContext(JWindowSettings *settings, JDisplayContent *content, int ChromiumLogs ,bool ErrorS);
void DestroyContext(int pid, int signal);
void JwebviewTerminate(int pid);

//Version 2.5
int CreateContext(JWindowSettings *settings, JDisplayContent *content);
void DestroyContext(int pid, int signal);
void JwebviewTerminate(int pid);

//Version 3.0
JWindow* JCreateContext(JWindowSize* WindowSize,JWindowSettings* WindowSettings,JDisplayContent* DisplayContent);
void JDestroyContext(JWindow* win, int signal);
void JWebviewTerminate(JWindow* win);
void JDebug(JWindow* win);
```

---

## Webview Window Flags

```
    int Zoom;
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
```

---

## Examples

### Render URL

```
#include "webview.h"

int main(void) {
    JWindowSize size = {800,600};
    JWindowSettings settings = { 
        .Zoom = 100, 
        .AddressBar = true,
        .ForceDarkMode = true,
        .Incognito = true,
        .SandBoxed =true
    }; 

    JDisplayContent content = { 
        .Ctype = URL, 
        .buffer = "https://www.google.com" 
    };

    JWindow* win = JCreateContext(&size, &settings, &content);
    JWebviewTerminate(win);
}
```

---

### Render HTML

```
#include "webview.h"

int main(void) {
    JWindowSize size = {800,600};
    JWindowSettings settings = { 
        .Zoom = 100, 
        .AddressBar = true,
        .ForceDarkMode = true,
        .Incognito = true,
        .SandBoxed =true
    }; 

    JDisplayContent content = { 
        .Ctype = DOCUMENT, 
        .buffer = "idk.html" 
    };

    JWindow* win = JCreateContext(&size, &settings, &content);
    JWebviewTerminate(win);
}
```

---

## Supported Platforms

* Linux

---

## Compiling

```bash
gcc target.c -lwebview -o target
```

---

## Versions
- 1.0 Release
- 2.0 New API more safety & control
- 2.5 more minimalism to CreateContext + new flags introduced
- 3.0 fixed some name inconsistencies + introduced objects + #define works
