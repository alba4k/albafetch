#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdbool.h>

struct Config {
    bool title_color;
    bool os_arch;
    bool de_type;
    bool shell_path;
    bool cpu_brand;
    bool cpu_freq;
    bool cpu_count;
    bool gpu_brand;
    bool mem_perc;
    bool pkg_mgr;
    bool pkg_pacman;
    bool pkg_dpkg;
    bool pkg_rpm;
    bool pkg_flatpak;
    bool pkg_snap;
    bool pkg_pip;
    bool pkg_brew;
    bool loc_localdomain;
    bool loc_docker;

    bool align;
    bool bold;
    char color[8];
};

extern struct Config config;

void print_line(char **logo, unsigned *line);

void unescape(char *str);

int max(const int *nums, unsigned const int lenght);

// needed for 
struct MemoryStruct {
  char *memory;
  size_t size;
};

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

// TODO
// this will be removed once I write a decent logic for the single modules
int separator(char *dest);
int spacing(char *dest);
int title(char *dest);

#endif // UTILS_H