#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "v3_0Jwebview.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <linux/limits.h>
#include <dirent.h>

JWindow* JCreateContext(JWindowSize* WindowSize,JWindowSettings* WindowSettings,JDisplayContent* DisplayContent){
    JWindow* win = (JWindow*)malloc(sizeof(JWindow));
    if (win != NULL) {
        char* browser_path = NULL;
        bool flatpak = false;
        char app_arg[1024];
        char document_arg[1024];

        char* browser_paths[] = {
            "/usr/bin/chromium",
            "/usr/bin/chromium-browser",
            "/usr/bin/google-chrome",
            "/usr/bin/google-chrome-stable",
            NULL
        };

        for (int i = 0; browser_paths[i]; i++) {
            if (access(browser_paths[i], X_OK) == 0) {
                browser_path = browser_paths[i];
                break;
            }
        }

        if (!browser_path) {
            if (access("/usr/bin/flatpak", X_OK) == 0) {
                browser_path = "flatpak";
                flatpak = true;
            }
            else {
                write(2, "Error: no browser found\n", 25);
                write(2,"try flatpak install org.chromium.Chromium or installing chromium through your package manager\n", 100);
                _exit(1);
            }
        }

        pid_t win_pid = fork();
        if (win_pid == 0) {
            char *argv[1024];
            int i = 0;
            if (!flatpak) {
                argv[i++] = "chromium";
            }
            else{
                argv[i++] = "flatpak";
                argv[i++] = "run";
                argv[i++] = "org.chromium.Chromium";
            }

            if (!WindowSettings->SandBoxed)
                argv[i++] = "--no-sandbox"; 

            if (!WindowSettings->ChromiumLogs) { 
                argv[i++] = "--log-level=3"; 
                argv[i++] = "--disable-logging"; 
                argv[i++] = "--disable-breakpad"; 
                argv[i++] = "--disable-crash-reporter"; 
            } 
            
            // modes
            if (WindowSettings->KioskMode) 
                argv[i++] = "--kiosk"; 
            else if (WindowSettings->FullScreen) 
                argv[i++] = "--start-fullscreen"; 

            if (WindowSettings->Incognito) 
                argv[i++] = "--incognito"; 

            if (!WindowSettings->GPU) 
                argv[i++] = "--disable-gpu";

            if (!WindowSettings->AddressBar) 
                argv[i++] = "--app-auto-launched";
                
            if (WindowSettings->ForceDarkMode) 
                argv[i++] = "--force-dark-mode"; 

            if (WindowSettings->HideScrollBars) 
                argv[i++] = "--hide-scrollbars"; 

            if (WindowSettings->DisableDevTools) {
                argv[i++] = "--disable-dev-tools"; 
                argv[i++] = "--no-first-run"; 
                argv[i++] = "--disable-infobars"; 
                argv[i++] = "--disable-session-crashed-bubble";
            }

            if (DisplayContent->Ctype == URL) {
                snprintf(app_arg, sizeof(app_arg),"--app=%s",DisplayContent->buffer);
                argv[i++] = app_arg;
            }
            else if (DisplayContent->Ctype == DOCUMENT) {
                char cwd[PATH_MAX];
                getcwd(cwd,sizeof(cwd));
                strcat(cwd,"/");
                strcat(cwd,DisplayContent->buffer);
                snprintf(document_arg,sizeof(document_arg), "--app=file://%s", cwd);
                argv[i++] = document_arg;
            }
            argv[i] = NULL;
            execvp(browser_path, argv);
            _exit(1);
        }
        strcpy(win->app_arg,app_arg);
        strcpy(win->browser_path,browser_path);
        win->pid = win_pid;
        win->height = WindowSize->height;
        win->width = WindowSize->width;
    }
    return win;
}

void JDestroyContext(JWindow* win, int signal) {
    if (win->pid <= 0) return;

    int kill_signal;
    switch(signal) {
        case JSIGINT:  kill_signal = SIGINT;  break;
        case JSIGKILL: kill_signal = SIGKILL; break;
        case JSIGTERM: kill_signal = SIGTERM; break;
        default:       kill_signal = SIGTERM; break;
    }

    kill(win->pid, kill_signal);
}

void JWebviewTerminate(JWindow* win) {
    if (win != NULL) {
        int status;
        waitpid(win->pid, &status, 0);
        free(win);
    }
}

void JDebug(JWindow* win) {
    printf("==========Debug Info==========\n");
    printf("Version: %s\n", JVERSION);
    printf("Pid: %d\n", win->pid);
    printf("Width: %d\n", win->width);
    printf("Height: %d\n", win->height);
}