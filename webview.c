#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "webview.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <linux/limits.h>

char Title[128];

static void WriteLaunchError(bool ErrorS, const char *msg) {
    if (ErrorS == 1) {
        write(2, msg, strlen(msg));
    }
}

int CreateContext(JWindowSettings *settings, JDisplayContent *content, int ChromiumLogs, bool ErrorS) {
    if (settings == NULL || content == NULL) {
        WriteLaunchError(ErrorS, "CreateContext got NULL args!\n");
        return -1;
    }

    if (ErrorS) {
        int fd = open("JEB.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            WriteLaunchError(ErrorS, "Failed to create log file!\n");
            return -1;
        }
        lseek(fd, 0, SEEK_SET);
        if (dup2(fd, STDERR_FILENO) < 0) {
            WriteLaunchError(ErrorS, "Dup2 failed!\n");
            close(fd);
            return -1;
        }
        close(fd);
    }

    char size_str[32] = "";
    if (settings->WindowSize.width > 0 && settings->WindowSize.height > 0) {
        snprintf(size_str, sizeof(size_str), "--window-size=%d,%d",
                 settings->WindowSize.width, settings->WindowSize.height);
    }

    char *chromium_paths[] = {
        "/usr/bin/chromium",
        "/usr/bin/chromium-browser",
        "/snap/bin/chromium",
        "/usr/bin/google-chrome",
        "/usr/bin/google-chrome-stable",
        "/opt/google/chrome/chrome",
        NULL
    };
    
    int chromium_found = 0;
    char *chromium_bin = NULL;
    for (int i = 0; chromium_paths[i] != NULL; i++) {
        if (access(chromium_paths[i], F_OK) == 0) {
            chromium_found = 1;
            chromium_bin = chromium_paths[i];
            break;
        }
    }
    
    if (!chromium_found) {
        WriteLaunchError(ErrorS, "Chromium/Chrome not found! Install chromium or google-chrome\n");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        WriteLaunchError(ErrorS, "Fork failed!\n");
        return -1;
    }
    
    if (pid == 0) {
        char *argv[30];
        int i = 0;
        
        argv[i++] = "chromium";
        
        // Base flags
        argv[i++] = "--no-sandbox";
        
        if (ChromiumLogs == 0) {
            argv[i++] = "--log-level=0";
            argv[i++] = "--disable-logging";
            argv[i++] = "--disable-breakpad";
            argv[i++] = "--disable-crash-reporter";
            argv[i++] = "--silent-launch";
            argv[i++] = "--disable-background-networking";
        }
        
        if (settings->Incognito == 1) {
            argv[i++] = "--incognito";
        }
        
        if (settings->DisableGPU == 1) {
            argv[i++] = "--disable-gpu";
        }
        
        if (size_str[0] != '\0') {
            argv[i++] = size_str;
        }
        
        char app_arg[1024];
        if (content->Ctype == URL) {
            snprintf(app_arg, sizeof(app_arg), "--app=%s", content->buffer);
        } else if (content->Ctype == DOCUMENT) {
            char abs_path[512];
            if (content->buffer[0] == '/') {
                snprintf(abs_path, sizeof(abs_path), "%s", content->buffer);
            } else {
                char cwd[512];
                getcwd(cwd, sizeof(cwd));
                snprintf(abs_path, sizeof(abs_path), "%s/%s", cwd, content->buffer);
            }
            snprintf(app_arg, sizeof(app_arg), "--app=file://%s", abs_path);
        } else {
            WriteLaunchError(ErrorS, "Invalid content type!\n");
            _exit(1);
        }
        argv[i++] = app_arg;
        argv[i] = NULL;

        //redirect stderr to /dev/null
        if (ChromiumLogs == 0) {
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull >= 0) {
                dup2(devnull, STDERR_FILENO);
                close(devnull);
            }
        }
        
        execv(chromium_bin, argv);
        _exit(1);
    }
    return pid;
}

void DestroyContext(int pid, int debug, int signal) {
    if (pid <= 0) return;
    
    int kill_signal;
    switch(signal) {
        case JSIGINT:  kill_signal = SIGINT;  break;
        case JSIGKILL: kill_signal = SIGKILL; break;
        case JSIGTERM: kill_signal = SIGTERM; break;
        default:       kill_signal = SIGTERM; break;
    }
    
    kill(pid, kill_signal);
    
    int status;
    waitpid(pid, &status, 0);
    
    if (debug) {
        if (WIFEXITED(status))
            printf("Process exited with status %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("Process killed by signal %d\n", WTERMSIG(status));
    }
}

void JwebviewTerminate(int pid) {
    int status;
    waitpid(pid, &status, 0);
}#include <stdio.h>
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
        int fd = open("Webview.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
