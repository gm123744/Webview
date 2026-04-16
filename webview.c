#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "webview.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <linux/limits.h>

#if defined(JVERSION_2_0)
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

void DestroyContext(int pid, int signal) {
    if (pid <= 0) return;
    
    int kill_signal;
    switch(signal) {
        case JSIGINT:  kill_signal = SIGINT;  break;
        case JSIGKILL: kill_signal = SIGKILL; break;
        case JSIGTERM: kill_signal = SIGTERM; break;
        default:       kill_signal = SIGTERM; break;
    }
    
    kill(pid, kill_signal);
}

void JwebviewTerminate(int pid) {
    int status;
    waitpid(pid, &status, 0);
}

#endif

#if defined(JVERSION_2_5)
int CreateContext(JWindowSettings *settings, JDisplayContent *content) {
    if (settings == NULL || content == NULL) {
        write(2, "CreateContext got NULL args!\n", 30);
        return -1;
    }

    if (settings->JWLOG) {
        int fd = open("JEB.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            write(2, "Failed to create log file!\n", 28);
            return -1;
        }
        if (dup2(fd, STDERR_FILENO) < 0) {
            write(2, "dup2 failed!\n", 13);
            close(fd);
            return -1;
        }
        close(fd);
    }

    char size_str[64] = "";
    if (settings->WindowSize.width > 0 && settings->WindowSize.height > 0) {
        snprintf(size_str, sizeof(size_str),
                 "--window-size=%d,%d",
                 settings->WindowSize.width,
                 settings->WindowSize.height);
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

    char *chromium_bin = NULL;
    for (int i = 0; chromium_paths[i]; i++) {
        if (access(chromium_paths[i], F_OK) == 0) {
            chromium_bin = chromium_paths[i];
            break;
        }
    }

    if (!chromium_bin) {
        write(2, "Chromium/Chrome not found!\n", 28);
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        write(2, "Fork failed!\n", 13);
        return -1;
    }

    if (pid == 0) {
        char *argv[64];
        int i = 0;

        argv[i++] = "chromium";
        argv[i++] = "--no-sandbox";

        // logging
        if (!settings->ChromiumLogs) {
            argv[i++] = "--log-level=3";
            argv[i++] = "--disable-logging";
            argv[i++] = "--disable-breakpad";
            argv[i++] = "--disable-crash-reporter";
        }

        // modes
        if (settings->KioskMode) {
            argv[i++] = "--kiosk";
        } else if (settings->FullScreen) {
            argv[i++] = "--start-fullscreen";
        }

        if (settings->Incognito) {
            argv[i++] = "--incognito";
        }

        if (!settings->GPU) {
            argv[i++] = "--disable-gpu";
        }

        if (!settings->AddressBar) {
            argv[i++] = "--app-auto-launched";
        }

        if (size_str[0]) {
            argv[i++] = size_str;
        }

        if (settings->DisableGoogleFeatures) {
            argv[i++] = "--disable-sync";
            argv[i++] = "--disable-translate";
            argv[i++] = "--disable-features=TranslateUI";
            argv[i++] = "--disable-component-update";
            argv[i++] = "--metrics-recording-only";
        }

        if (settings->ForceDarkMode) {
            argv[i++] = "--force-dark-mode";
        }

        if (settings->HideScrollBars) {
            argv[i++] = "--hide-scrollbars";
        }

        if (settings->DisableDevTools) {
            argv[i++] = "--disable-dev-tools";
        }

        argv[i++] = "--no-first-run";
        argv[i++] = "--disable-infobars";
        argv[i++] = "--disable-session-crashed-bubble";

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
            write(2, "Invalid content type!\n", 23);
            _exit(1);
        }

        argv[i++] = app_arg;
        argv[i] = NULL;

        if (!settings->ChromiumLogs) {
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull >= 0) {
                dup2(devnull, STDERR_FILENO);
                close(devnull);
            }
        }

        execv(chromium_bin, argv);
        write(2, "execv failed!\n", 14);
        _exit(1);
    }

    return pid;
}

void DestroyContext(int pid, int signal) {
    if (pid <= 0) return;
    
    int kill_signal;
    switch(signal) {
        case JSIGINT:  kill_signal = SIGINT;  break;
        case JSIGKILL: kill_signal = SIGKILL; break;
        case JSIGTERM: kill_signal = SIGTERM; break;
        default:       kill_signal = SIGTERM; break;
    }
    
    kill(pid, kill_signal);
}

void JwebviewTerminate(int pid) {
    int status;
    waitpid(pid, &status, 0);
}
#endif
