#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "webview.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

char Title[128];

static void WriteLaunchError(bool ErrorS, const char *msg) {
    if (ErrorS == 1) {
        write(2, msg, strlen(msg));
    }
}

void WindowSizeInit(JWindowSize *sz, int width, int height) {
    sz->width = width;
    sz->height = height;
}

int WindowSettingsInit(JWindowSettings *settings, int addressbar, int fullscreen, int incognito, int kioskmode) {
    settings->AddressBar = addressbar;
    settings->FullScreen = fullscreen;
    settings->Incognito = incognito;
    settings->KioskMode = kioskmode;
    settings->Zoom = 100;
    settings->DisableGPU = 0;
    
    // Set defaults if not already set
    if (settings->WindowSize.width == 0) settings->WindowSize.width = 1024;
    if (settings->WindowSize.height == 0) settings->WindowSize.height = 768;

    return (addressbar ? 1 : 0) | (fullscreen ? 2 : 0) | (incognito ? 4 : 0) | (kioskmode ? 8 : 0);
}

int DisplayContent(JDisplayContent *content, int Ctype, char *buffer) {
    content->Ctype = Ctype;
    strcpy(content->buffer, buffer);

    if (Ctype == URL)
        return 0;
    else if (Ctype == DOCUMENT)
        return 1;
    else
        return -1;
}

void CreateContext(JWindowSettings *settings, JDisplayContent *content, bool ErrorS) {
    if (settings == NULL || content == NULL) {
        WriteLaunchError(ErrorS, "CreateContext got NULL args!\n");
        return;
    }

    if (ErrorS) {
        int fd = open("JEB.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            WriteLaunchError(ErrorS, "Failed to create log file!\n");
            return;
        }
        lseek(fd, 0, SEEK_SET);
        if (dup2(fd, STDERR_FILENO) < 0) {
            WriteLaunchError(ErrorS, "Dup2 failed!\n");
            close(fd);
            return;
        }
        close(fd);
    }

    // Prepare size flag
    char size_str[32] = "";
    if (settings->WindowSize.width > 0 && settings->WindowSize.height > 0) {
        snprintf(size_str, sizeof(size_str), "--window-size=%d,%d",
                 settings->WindowSize.width, settings->WindowSize.height);
    }

    if (access("/usr/bin/chromium", F_OK) != 0 && access("/usr/bin/chromium-browser", F_OK) != 0) {
        WriteLaunchError(ErrorS, "Chromium not found! Install chromium-browser\n");
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        WriteLaunchError(ErrorS, "Fork failed!\n");
        return;
    }
    
    if (pid == 0) {
        if (content->Ctype == URL) {
            char app_arg[1024];
            snprintf(app_arg, sizeof(app_arg), "--app=%s", content->buffer);
            if (size_str[0] != '\0')
                execlp("chromium", "chromium", "--no-sandbox", "--incognito",size_str, app_arg, NULL);
            else
                execlp("chromium", "chromium", "--no-sandbox","--incognito", app_arg, NULL);
            _exit(1);
        }
        else if (content->Ctype == DOCUMENT) {
            char app_arg[512];
            snprintf(app_arg, sizeof(app_arg), "--app=file://%s", content->buffer);
            
            if (size_str[0] != '\0')
                execlp("chromium", "chromium", "--no-sandbox","--incognito", size_str, app_arg, NULL);
            else
                execlp("chromium", "chromium", "--no-sandbox","--incognito", app_arg, NULL);
            _exit(1);
        }
        
        WriteLaunchError(ErrorS, "Invalid content type!\n");
        _exit(1);
    }
}
