#include "info_module.h"
#include "../config/config.h"

#include <stdlib.h>
#include <string.h>

static const SModule mod_table[] = {
    // {"identifier", label, func},
    {"separator", config.separator_prefix, NULL, NULL},
    {"space", config.spacing_prefix, NULL, NULL},
    {"title", config.title_prefix, NULL, NULL},
    {"user", config.user_prefix, user, NULL},
    {"hostname", config.hostname_prefix, hostname, NULL},
    {"uptime", config.uptime_prefix, uptime, NULL},
    {"os", config.os_prefix, os, NULL},
    {"kernel", config.kernel_prefix, kernel, NULL},
    {"desktop", config.desktop_prefix, desktop, NULL},
    {"gtk_theme", config.gtk_theme_prefix, gtkTheme, NULL},
    {"icon_theme", config.icon_theme_prefix, iconTheme, NULL},
    {"cursor_theme", config.cursor_theme_prefix, cursorTheme, NULL},
    {"shell", config.shell_prefix, shell, NULL},
    {"login_shell", config.login_shell_prefix, loginShell, NULL},
    {"term", config.term_prefix, term, NULL},
    {"packages", config.pkg_prefix, packages, NULL},
    {"host", config.host_prefix, host, NULL},
    {"bios", config.bios_prefix, bios, NULL},
    {"cpu", config.cpu_prefix, cpu, NULL},
    {"gpu", config.gpu_prefix, gpu, NULL},
    {"memory", config.mem_prefix, memory, NULL},
    {"swap", config.swap_prefix, swap, NULL},
    {"public_ip", config.pub_prefix, publicIp, NULL},
    {"local_ip", config.loc_prefix, localIp, NULL},
    {"pwd", config.pwd_prefix, pwd, NULL},
    {"date", config.date_prefix, date, NULL},
    {"battery", config.bat_prefix, battery, NULL},
    {"colors", config.colors_prefix, colors, NULL},
    {"light_colors", config.colors_light_prefix, lightColors, NULL},
};

// add a module containing id to array and return it
SModule *addModule(SModule *array, char *id) {
    SModule *new = malloc(sizeof(SModule));
    if(new == NULL)
        return NULL;

    new->id = strdup(id);
    if(new->id == NULL) {
        free(new);
        return NULL;
    }

    new->label = NULL;
    new->func = NULL;

    for(size_t i = 0; i < sizeof(mod_table) / sizeof(mod_table[0]); ++i)
        if(strcmp(mod_table[i].id, id) == 0) {
            new->label = mod_table[i].label;
            new->func = mod_table[i].func;
        }

    new->next = NULL;

    SModule *last = array;
    for(SModule *current = array; current->next; current = current->next)
        last = current->next;
    last->next = new;

    return new;
}

// free every module in array
void destroyArray(SModule *array) {
    SModule *current = array;
    SModule *next;

    while(current) {
        next = current->next;
        free(current->id);
        free(current);
        current = next;
    }
}
