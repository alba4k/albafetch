#include "info.h"
#include "../config/config.h"
#include "../utils.h"

#include <string.h>

// just here to stop vscode from complaining about DT_DIR
#ifndef __USE_MISC
#define __USE_MISC
#endif

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// get the number of installed packages
int packages(char *dest) {
    dest[0] = 0;
    char buf[256] = "", str[128] = "", path[256] = "";
    DIR *dir;
    struct dirent *entry;
    unsigned count = 0;
    bool done = false;

    #ifndef __APPLE__   // package managers that won't run on macOS
        FILE *fp;

        path[0] = 0;
        if(getenv("PREFIX"))
            strncpy(path, getenv("PREFIX"), 255);
        strncat(path, "/var/lib/pacman/local", 256-strlen(path));
        if(_pkg_pacman && (dir = opendir(path)) != NULL) {
            while((entry = readdir(dir)) != NULL)
                if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                    ++count;
            closedir(dir);

            if(count) {
                snprintf(dest, 255 - strlen(buf), "%s%u%s", done ? ", " : "", count, _pkg_mgr ? " (pacman)" : "");
                done = true;
            }
        }

        path[0] = 0;
        if(getenv("PREFIX"))
            strncpy(path, getenv("PREFIX"), 255);
        strncat(path, "/var/lib/dpkg/status", 256-strlen(path));
        if(_pkg_dpkg && (fp = fopen(path, "r")) != NULL) {   // alternatively, I could use "dpkg-query -f L -W" and strlen
            fseek(fp, 0, SEEK_END);
            size_t len = (size_t)ftell(fp);
            rewind(fp);

            char *dpkg_list = malloc(len+1);
            dpkg_list[fread(dpkg_list, 1, len, fp)] = 0;

            fclose(fp);

            count = 0;
            char *ptr = dpkg_list;
            // this will be wrong if some package (for whatever reason) does not have "\nInstalled-Size: "
            // or if some package has it in the package description
            while((ptr = strstr(ptr, "\nInstalled-Size: "))) {
                ++count;
                ptr += 17;
            }
            free(dpkg_list);

            if(count) {
                snprintf(buf, 256, "%u%s", count, _pkg_mgr ? " (dpkg)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }
        }

        path[0] = 0;
        if(getenv("PREFIX"))
            strncpy(path, getenv("PREFIX"), 255);
        strncat(path, "/var/lib/rpm/rpmdb.sqlite", 256-strlen(path));
        if(_pkg_rpm && access(path, F_OK) == 0) {
            char *args[] = {"sqlite3", path, "SELECT count(*) FROM Packages", NULL};
            exec_cmd(str, 16, args);

            if(str[0] != '0' && str[0]) {
                snprintf(buf, 255 - strlen(buf), "%s%s%s", done ? ", " : "", str, _pkg_mgr ? " (rpm)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }
        }

        path[0] = 0;
        if(getenv("PREFIX"))
            strncpy(path, getenv("PREFIX"), 255);
        strncat(path, "/var/lib/flatpak/runtime", 256-strlen(path));
        if(_pkg_flatpak && (dir = opendir(path)) != NULL) {
            count = 0;
            while((entry = readdir(dir)) != NULL)
                if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                    ++count;
            closedir(dir);

            if(count) {
                snprintf(buf, 255 - strlen(buf), "%s%u%s", done ? ", " : "", count, _pkg_mgr ? " (flatpak)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }
        }

        path[0] = 0;
        if(getenv("PREFIX"))
            strncpy(path, getenv("PREFIX"), 255);
        strncat(path, "/var/snap", 256-strlen(path));
        if(_pkg_snap && (dir = opendir(path)) != NULL) {
            count = 0;
            while((entry = readdir(dir)) != NULL)
                if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                    ++count;
            closedir(dir);

            if(count) {
                snprintf(buf, 255 - strlen(buf), "%s%u%s", done ? ", " : "", count, _pkg_mgr ? " (snap)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }
        }
    #endif

    if(_pkg_brew && (access("/usr/local/bin/brew", F_OK) == 0 || access("/opt/homebrew/bin/brew", F_OK) == 0 || access("/bin/brew", F_OK) == 0)) {
        char *args[] = {"brew", "--cellar", NULL};
        exec_cmd(str, 16, args);

        if(str[0]) {
            if((dir = opendir(str)) != NULL) {
                count = 0;

                while((entry = readdir(dir)) != NULL)
                    if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
                        ++count;
                closedir(dir);

                if(count) {
                    snprintf(buf, 256, "%s%u%s", done ? ", " : "", count, _pkg_mgr ? " (brew)" : "");
                    done = true;
                    strncat(dest, buf, 256 - strlen(dest));
                }
            }
        }
    }


    path[0] = 0;
    if(getenv("PREFIX"))
        strncpy(path, getenv("PREFIX"), 255);
    strncat(path, "/bin/pip", 256-strlen(path));
    if(_pkg_pip && access(path, F_OK) == 0) {
        char *args[] = {"sh", "-c", "pip list 2>/dev/null | wc -l", NULL};
        exec_cmd(str, 16, args);
        
        if(str[0] != '0' && str[0]) {
            snprintf(buf, 255 - strlen(buf), "%s%d%s", done ? ", " : "", atoi(str)-2, _pkg_mgr ? " (pip)" : "");
            done = true;
            strncat(dest, buf, 256 - strlen(dest));
        }
    }

    return !done;
}
