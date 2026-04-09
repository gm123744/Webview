#   Jeb Webview Tutorial

jebs Webview is a small launcher which uses the Chromium browser to render HTML files and URL files with minimal syntax.

---

## Webview Functions

```
int CreateContext(JWindowSettings *settings, JDisplayContent *content, int ChromiumLogs ,bool ErrorS);
void DestroyContext(int pid,int debug, int signal);
void JwebviewTerminate(int pid);
```

---

## Webview Window Flags

```
    ADDRESSBAR
    FULLSCREEN
    INCOGNITO
    KIOSKMODE
    ENABLE_GPU
    CHROMIUM_LOGS
    DEBUG
    JEB_LOG
    NO_ADDRESSBAR
    NO_FULLSCREEN
    NO_INCOGNITO
    NO_KIOSKMODE
    DISABLE_GPU
    NO_CHROMIUM_LOGS 
    NO_DEBUG
    NO_WEBVIEW_LOG
```

---

## Examples

### Render URL

```
#include <webview.h>
#include <stdio.h>

int main() {
    JWindowSettings settings = {
        .AddressBar = NO_ADDRESSBAR,
        .FullScreen = NO_FULLSCREEN,
        .Incognito = INCOGNITO,
        .KioskMode = NO_KIOSKMODE,
        .DisableGPU = ENABLE_GPU,
        .Zoom = 100,
        .WindowSize = {800,600}
    };
    JDisplayContent content = {
        .Ctype = URL,
        .buffer = "https://vscode.dev/"
    };

    int pid = CreateContext(&settings,&content, NO_CHROMIUM_LOGS ,NO_WEBVIEW_LOG);

    JwebviewTerminate(pid);
    return 0;
}
```

---

### Render HTML

```
#include "webview.h"
#include <stdio.h>

int main() {
    JWindowSettings settings = {
        .AddressBar = NO_ADDRESSBAR,
        .FullScreen = NO_FULLSCREEN,
        .Incognito = INCOGNITO,
        .KioskMode = NO_KIOSKMODE,
        .DisableGPU = ENABLE_GPU,
        .Zoom = 100,
        .WindowSize = {800,600}
    };
    JDisplayContent content = {
        .Ctype = DOCUMENT,
        .buffer = "target.html"
    };

    int pid = CreateContext(&settings,&content, NO_CHROMIUM_LOGS ,NO_WEBVIEW_LOG);

    JwebviewTerminate(pid);
    return 0;
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
