#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// get the current terminal
int term(char *dest) {
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
        if(terminal == NULL)
            return ERR_NO_INFO;

        if(strcmp(terminal, "xterm-kitty") == 0)
            terminal = "Kitty";
        else if(strcmp(terminal, "alacritty") == 0)
            terminal = "Alacritty";
        else if(strcmp(terminal, "Apple_Terminal") == 0)
            terminal = "Apple Terminal";
    }

    if(_term_ssh && getenv("SSH_CONNECTION"))
        snprintf(dest, DEST_SIZE, "%s (SSH)", terminal);
    else
        safeStrncpy(dest, terminal, DEST_SIZE);

    if(_term_version) {
        char version[32] = "";
        if(strcmp(terminal, "Kitty") == 0) {
            char *argv[] = {"kitty", "--version", NULL};
            execCmd(version, sizeof(version), argv);
            char *end = strchr(version + 6, ' ');
            if(end != NULL) {
                *end = 0;
                strncat(dest, " ", DEST_SIZE - strlen(dest));
                strncat(dest, version + 6, DEST_SIZE - strlen(dest));
            }
        }
        else if(strcmp(terminal, "Alacritty") == 0) {
            char *argv[] = {"alacritty", "--version", NULL};
            execCmd(version, sizeof(version), argv);
            char *end = strchr(version + 10, ' ');
            if(end != NULL) {
                *end = 0;
                strncat(dest, " ", DEST_SIZE - strlen(dest));
                strncat(dest, version + 10, DEST_SIZE - strlen(dest));
            }
        }
        else if(strcmp(terminal, "Konsole") == 0) {
            char *argv[] = {"konsole", "--version", NULL};
            execCmd(version, sizeof(version), argv);
            strncat(dest, version + 7, DEST_SIZE - strlen(dest));
        }
    }

    return RET_OK;
}
