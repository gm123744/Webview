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
            if (WindowSettings->JWLOG) {
                int log_fd = open("jwebview.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (log_fd >= 0) {
                    dup2(log_fd, STDOUT_FILENO);
                    dup2(log_fd, STDERR_FILENO);
                    close(log_fd);
                }
            }
            
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
            
            if (WindowSettings->Zoom > 0 && WindowSettings->Zoom != 100) {
                float zoom_factor = WindowSettings->Zoom / 100.0f;
                char zoom_arg[32];
                snprintf(zoom_arg, sizeof(zoom_arg), "--force-device-scale-factor=%.2f", zoom_factor);
                argv[i++] = zoom_arg;
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
            
            if (WindowSettings->DisableGoogleFeatures) {
                argv[i++] = "--disable-background-networking";
                argv[i++] = "--disable-sync";
                argv[i++] = "--disable-default-apps";
                argv[i++] = "--no-service-autorun";
                argv[i++] = "--disable-component-extensions-with-background-pages";
                argv[i++] = "--disable-features=GoogleNow,GoogleSearch,GoogleDrive,GoogleCloudMessaging,ChromeWhatsNew";
                argv[i++] = "--disable-component-update";
                argv[i++] = "--disable-domain-reliability";
                argv[i++] = "--disable-demo-mode";
                argv[i++] = "--disable-background-timer-throttling";
            }
            
            if (WindowSize->width > 0 && WindowSize->height > 0) {
                char size_arg[64];
                char position_arg[64];
                snprintf(size_arg, sizeof(size_arg), "--window-size=%d,%d", WindowSize->width, WindowSize->height);
                snprintf(position_arg, sizeof(position_arg), "--window-position=0,0");
                argv[i++] = size_arg;
                argv[i++] = position_arg;
                argv[i++] = "--new-window";
            }
            
            // Load content
            if (DisplayContent->Ctype == URL) {
                argv[i++] = DisplayContent->buffer;
            }
            else if (DisplayContent->Ctype == DOCUMENT) {
                char cwd[PATH_MAX];
                getcwd(cwd, sizeof(cwd));
                char file_url[PATH_MAX + 20];
                snprintf(file_url, sizeof(file_url), "file://%s/%s", cwd, DisplayContent->buffer);
                argv[i++] = file_url;
            }
            
            argv[i] = NULL;
            execvp(browser_path, argv);
            _exit(1);
        }
        strcpy(win->browser_path, browser_path);
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
