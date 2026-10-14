#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// get the current terminal
SInfo *term(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    const char *terminal = NULL;

    const char *terminals[][2] = {// {"ENVIRONMENT_VARIABLE", "terminal"},
                                  {"ALACRITTY_WINDOW_ID", "Alacritty"}, {"KITTY_PID", "Kitty"},           {"VSCODE_INJECTION", "VS Code"},
                                  {"TERMUX_VERSION", "Termux"},         {"KONSOLE_VERSION", "Konsole"},   {"GNOME_TERMINAL_SCREEN", "GNOME Terminal"},
                                  {"WT_SESSION", "Windows Terminal"},   {"TERMINATOR_UUID", "Terminator"}};

    for(size_t i = 0; i < sizeof(terminals) / sizeof(terminals[0]); ++i)
        if(getenv(terminals[i][0]))
            terminal = terminals[i][1];

    if(terminal == NULL) {
        terminal = getenv("TERM_PROGRAM");
        if(terminal == NULL)
            terminal = getenv("TERM");
        if(terminal == NULL) {
            res->retval = ERR_NO_INFO;
            return res;
        }

        if(strcmp(terminal, "xterm-kitty") == 0)
            terminal = "Kitty";
        else if(strcmp(terminal, "alacritty") == 0)
            terminal = "Alacritty";
        else if(strcmp(terminal, "Apple_Terminal") == 0)
            terminal = "Apple Terminal";
    }

    res->string = strdup(terminal);
    if(res->string == NULL) {
        res->retval = ERR_OOM;
        return res;
    }
    
    if(_term_ssh && getenv("SSH_CONNECTION")) {
        char *tmpptr = realloc(res->string, strlen(res->string) + strlen(" (SSH)") + 1);
        if(tmpptr) {
            res->string = tmpptr;
            strcat(res->string, " (SSH)");
        }
    }

    if(_term_version) {
        char version[32] = "";
        if(strcmp(terminal, "Kitty") == 0) {
            char *argv[] = {"kitty", "--version", NULL};
            execCmd(version, sizeof(version), argv);
            if(strlen(version) < 7)
                return res;

            memmove(version, version + 6, strlen(version + 6) + 1);
            char *end = strchr(version, ' ');
            if(end != NULL)
                *end = 0;
        }
        else if(strcmp(terminal, "Alacritty") == 0) {
            char *argv[] = {"alacritty", "--version", NULL};
            execCmd(version, sizeof(version), argv);
            if(strlen(version) < 11)
                return res;

            memmove(version, version + 10, strlen(version + 10) + 1);
            char *end = strchr(version + 10, ' ');
            if(end != NULL)
                *end = 0;
        }
        else if(strcmp(terminal, "VS Code") == 0) {
            char *argv[] = {"code", "--version", NULL};
            execCmd(version, sizeof(version), argv);
            if(!version[0])
                return res;
            char *end = strchr(version, '\n');
            if(end != NULL)
                *end = 0;
        }
        else if(strcmp(terminal, "Konsole") == 0) {
            char *argv[] = {"konsole", "--version", NULL};
            execCmd(version, sizeof(version), argv);
            if(strlen(version) < 8)
                return res;

            memmove(version, version + 7, strlen(version + 7) + 1);
        }

        if(version[0] == 0)
            return res;

        char *tmpptr = realloc(res->string, strlen(res->string) + strlen(version) + 2);
        if(tmpptr) {
            res->string = tmpptr;
            strcat(res->string, " ");
            strcat(res->string, version);
        }
    }

    return res;
}
