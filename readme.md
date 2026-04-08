# Webview tutorial

Webview is a small library which uses the chromium browser to render html files and url files with minimal syntax

## Webview functions
void WindowSizeInit(JWindowSize *sz, int width, int height);
int WindowSettingsInit(JWindowSettings *settings, int addressbar, int fullscreen, int incognito, int kioskmode);
int DisplayContent(JDisplayContent *content, int Ctype, char* buffer);
void CreateContext(JWindowSettings *settings, JDisplayContent *content, bool LOG);

## Examples

'''Render URL
#include <webview.h>
#include <stdio.h>

int main() {
    JWindowSettings settings;
    JDisplayContent content;

    WindowSizeInit(&settings.WindowSize, 800, 600); //set window size for chromium
    WindowSettingsInit(&settings, NO_ADDRESSBAR, NO_FULLSCREEN, INCOGNITO, NO_KIOSKMODE); //available browser modes
    DisplayContent(&content, URL, "https://www.youtube.com");
    CreateContext(&settings, &content, NO_WEBVIEW_LOG); //Create the browser window and display content
    
    return 0;
}
'''

'''Render html
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
'''
## Compiling

gcc target.c -lwebview -o target

## Supported platform

- Linux
