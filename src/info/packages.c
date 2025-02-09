#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// just here to stop vscode from complaining about DT_DIR
#ifndef __USE_MISC
#define __USE_MISC
#endif

#include <dirent.h>
#include <sqlite3.h>
#include <sys/wait.h>

#include "info.h"
#include "../config/config.h"
#include "../optdeps/optdeps.h"
#include "../utils/wrappers.h"

/*
 * I know that I could probably use specific libraries (like ALPM
 * or some RPM stuff) and add them as optional dependencies, however:
 *   - I'm too lazy to do that
 *   - that would be way slower than just counting directories
 */

// get the number of installed packages
int packages(char *dest) {
    dest[0] = 0;
    char buf[DEST_SIZE] = "", str[128] = "";
    DIR *dir;
    struct dirent *entry;
    unsigned count = 0;
    bool done = false;

    #ifdef __linux__   // package managers that won't run on macOS
        FILE *fp;
        char *prefix = getenv("PREFIX");
        char path[256];

        path[0] = 0;
        if(prefix)
            safe_strncpy(path, prefix, 256);
        strncat(path, "/var/lib/pacman/local", 255-strlen(path));
        if(_pkg_pacman && (dir = opendir(path)) != NULL) {
            while((entry = readdir(dir)) != NULL)
                if(entry->d_type == DT_DIR && entry->d_name[0] != '.')
                    ++count;
            closedir(dir);

            if(count) {
                snprintf(dest, DEST_SIZE, "%u%s", count, _pkg_mgr ? " (pacman)" : "");
                done = true;
            }
        }

        path[0] = 0;
        if(prefix)
            safe_strncpy(path, prefix, 256);
        strncat(path, "/var/lib/dpkg/status", 255-strlen(path));
        if(_pkg_dpkg && (fp = fopen(path, "r")) != NULL) {
            char line[512];
            int count = 0;

            while(fgets(line, sizeof(line), fp)) {
                // check if the line starts with "Package:"
                if(strncmp(line, "Package:", 8) == 0) {
                    ++count;
                }
            }
            fclose(fp);

            if(count) {
                snprintf(buf, DEST_SIZE, "%u%s", count, _pkg_mgr ? " (dpkg)" : "");
                done = true;
                strncat(dest, buf, DEST_SIZE - strlen(dest));
            }
        }

        path[0] = 0;
        if(prefix)
            safe_strncpy(path, prefix, 256);
        strncat(path, "/var/lib/rpm/rpmdb.sqlite", 255-strlen(path));
        if(_pkg_rpm && access(path, F_OK) == 0) {
            sqlite3 *db;
            sqlite3_stmt *stmt;
            int count = 0;

            if(sqlite3_open(path, &db) != SQLITE_OK)
                goto skip;
            if(sqlite3_prepare_v2(db, "SELECT count(*) FROM Packages", -1, &stmt, NULL) != SQLITE_OK) {
                sqlite3_close(db);
                goto skip;
            }

            if (sqlite3_step(stmt) == SQLITE_ROW) {
                count = sqlite3_column_int(stmt, 0);

                if(count > 0) {
                    snprintf(buf, DEST_SIZE - strlen(buf), "%s%s%s", done ? ", " : "", str, _pkg_mgr ? " (rpm)" : "");
                    done = true;
                    strncat(dest, buf, DEST_SIZE - strlen(dest));
                }
            }

            sqlite3_finalize(stmt);
            sqlite3_close(db);

            skip: ;
        }

        path[0] = 0;
        if(prefix)
            safe_strncpy(path, prefix, 256);
        strncat(path, "/var/lib/flatpak/runtime", 255-strlen(path));
        if(_pkg_flatpak && (dir = opendir(path)) != NULL) {
            count = 0;
            while((entry = readdir(dir)) != NULL)
                if(entry->d_type == DT_DIR && entry->d_name[0] != '.')
                    ++count;
            closedir(dir);

            if(count) {
                snprintf(buf, DEST_SIZE - strlen(buf), "%s%u%s", done ? ", " : "", count, _pkg_mgr ? " (flatpak)" : "");
                done = true;
                strncat(dest, buf, DEST_SIZE - strlen(dest));
            }
        }

        path[0] = 0;
        if(prefix)
            safe_strncpy(path, prefix, 256);
        strncat(path, "/var/snap", 255-strlen(path));
        if(_pkg_snap && (dir = opendir(path)) != NULL) {
            count = 0;
            while((entry = readdir(dir)) != NULL)
                if(entry->d_type == DT_DIR && entry->d_name[0] != '.')
                    ++count;
            closedir(dir);

            if(count) {
                snprintf(buf, DEST_SIZE - strlen(buf), "%s%u%s", done ? ", " : "", count, _pkg_mgr ? " (snap)" : "");
                done = true;
                strncat(dest, buf, DEST_SIZE - strlen(dest));
            }
        }
    #endif

    if(_pkg_brew && (binary_in_path("brew"))) {
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
                    snprintf(buf, DEST_SIZE, "%s%u%s", done ? ", " : "", count, _pkg_mgr ? " (brew)" : "");
                    done = true;
                    strncat(dest, buf, DEST_SIZE - strlen(dest));
                }
            }
        }
    }

    if(_pkg_pip && binary_in_path("pip")) {
        char *args[] = {"sh", "-c", "pip list 2>/dev/null | wc -l", NULL};
        exec_cmd(str, 16, args);
        
        if(str[0] != '0' && str[0]) {
            snprintf(buf, DEST_SIZE - strlen(buf), "%s%d%s", done ? ", " : "", atoi(str)-2, _pkg_mgr ? " (pip)" : "");
            done = true;
            strncat(dest, buf, DEST_SIZE - strlen(dest));
        }
    }

    return !done;
}
