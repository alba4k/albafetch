#include "info.h"
#include "../utils.h"

#include <string.h>

#include <stdio.h>
#include <stdlib.h>

// get the current terminal
int term(char *dest) {
    // TODO: print terminal version (using env variables, parsing --version outputs, ...)
    char *terminal = NULL;

    char *terminals[][2] = {
     // {"ENVIRONMENT_VARIABLE", "terminal"},
        {"ALACRITTY_SOCKET", "alacritty"},
        {"KITTY_PID", "kitty"},
        {"VSCODE_INJECTION", "vscode"},
        {"TERMUX_VERSION", "termux"},
        {"KONSOLE_VERSION", "konsole"},
        {"GNOME_TERMINAL_SCREEN", "gnome-terminal"},
        {"WT_SESSION", "windows terminal"},
    };

    for(size_t i = 0; i < sizeof(terminals)/sizeof(terminals[0]); ++i)
        if(getenv(terminals[i][0]))
            terminal = terminals[i][1];

    if(!terminal) {
        terminal = getenv("TERM");
        if(!terminal)
            return 1;
        
        if(!strcmp(terminal, "xterm-kitty"))
            terminal = "kitty";
    }

    if(term_ssh && getenv("SSH_CONNECTION"))
        snprintf(dest, 256, "%s (SSH)", terminal);
    else
        strncpy(dest, terminal, 256);

    return 0;
}
