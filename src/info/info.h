#pragma once

#include "../utils/return.h"  // IWYU pragma: export

typedef struct {
    int retval;
    char *string;
} SInfo;

SInfo *user(void);
SInfo *ostname(void);
SInfo *uptime(void);
SInfo *os(void);
SInfo *kernel(void);
SInfo *desktop(void);
SInfo *gtkTheme(void);
SInfo *iconTheme(void);
SInfo *cursorTheme(void);
SInfo *shell(void);
SInfo *loginShell(void);
SInfo *term(void);
SInfo *packages(void);
SInfo *host(void);
SInfo *hostname(void);
SInfo *bios(void);
SInfo *cpu(void);
SInfo *gpu(void);
SInfo *memory(void);
SInfo *swap(void);
SInfo *publicIp(void);
SInfo *localIp(void);
SInfo *pwd(void);
SInfo *date(void);
SInfo *battery(void);
SInfo *colors(void);
SInfo *lightColors(void);
