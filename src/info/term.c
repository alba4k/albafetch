#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// get the current terminal
int term(char *dest) {
    // TODO: print terminal version (using env variables, parsing --version outputs, ...)
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

    return RET_OK;
}
