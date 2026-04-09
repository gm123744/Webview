#   Jeb Webview Tutorial

jebs Webview is a small launcher which uses the Chromium browser to render HTML files and URL files with minimal syntax.

---

## Webview Functions

```
void WindowSizeInit(JWindowSize *sz, int width, int height);
int WindowSettingsInit(JWindowSettings *settings, int addressbar, int fullscreen, int incognito, int kioskmode);
int DisplayContent(JDisplayContent *content, int Ctype, char* buffer);
void CreateContext(JWindowSettings *settings, JDisplayContent *content, bool LOG);
```

---

## Webview Window Flags

```
  ADDRESSBAR = 1,
  FULLSCREEN = 1,
  INCOGNITO = 1,
  KIOSKMODE = 1,
  NO_ADDRESSBAR = 0,
  NO_FULLSCREEN = 0,
  NO_INCOGNITO = 0,
  NO_KIOSKMODE = 0,
  NO_JEB_LOG = false
```

---

## Examples

### Render URL

```
#include <webview.h>
#include <stdio.h>

int main() {
    JWindowSettings settings;
    JDisplayContent content;

    WindowSizeInit(&settings.WindowSize, 800, 600); // set window size for Chromium
     WindowSettingsInit(&settings, NO_ADDRESSBAR, NO_FULLSCREEN, INCOGNITO, NO_KIOSKMODE); // available browser modes
     DisplayContent(&content, URL, "https://www.youtube.com");
    CreateContext(&settings, &content, NO_WEBVIEW_LOG); // Create the browser window and display content

    return 0;
}
```

---

### Render HTML

```
#include <webview.h>
#include <stdio.h>

int main() {
    JWindowSettings settings;
    JDisplayContent content;

    WindowSizeInit(&settings.WindowSize, 800, 600);
     WindowSettingsInit(&settings, NO_ADDRESSBAR, NO_FULLSCREEN, INCOGNITO, NO_KIOSKMODE);
     DisplayContent(&content, DOCUMENT, "path/to/local/file.html");
    CreateContext(&settings, &content, NO_WEBVIEW_LOG);

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
