#pragma once

#include "../utils/return.h"

#define DEST_SIZE 256

int user(char *dest);

int hostname(char *dest);

int uptime(char *dest);

int os(char *dest);

int kernel(char *dest);

int desktop(char *dest);

int gtkTheme(char *dest);

int iconTheme(char *dest);

int cursorTheme(char *dest);

int shell(char *dest);

int loginShell(char *dest);

int term(char *dest);

int packages(char *dest);

int host(char *dest);

int bios(char *dest);

int cpu(char *dest);

int gpu(char *dest);

int memory(char *dest);

int swap(char *dest);

int publicIp(char *dest);

int localIp(char *dest);

int pwd(char *dest);

int date(char *dest);

int battery(char *dest);

int colors(char *dest);

int lightColors(char *dest);
