#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// just here to stop vscode from complaining about DT_DIR
#ifndef __USE_MISC
#define __USE_MISC
#endif

#include <dirent.h>
#ifdef SQLITE3_EXISTS
#include <sqlite3.h>
#endif
#include <sys/wait.h>

#include "../config/config.h"
#include "../utils/wrappers.h"
#include "info.h"

/*
 * I know that I could probably use specific libraries (like ALPM
 * or some RPM stuff) and add them as optional dependencies, however:
 *   - I'm too lazy to do that
 *   - that would be way slower than just counting directories
 */

// get the number of installed packages
SInfo *packages(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    char buf[64] = "", buf2[sizeof(buf)] = "";

    DIR *dir;
    struct dirent *entry;
    int count = 0;
    bool done = false;

#ifdef __linux__ // package managers that won't run on macOS
    FILE *fp;
    char *prefix = getenv("PREFIX");
    char path[256];

    path[0] = 0;
    if(prefix)
        safeStrncpy(path, prefix, sizeof(path));
    strncat(path, "/var/lib/pacman/local", sizeof(path) - strlen(path) - 1);
    if(_pkg_pacman && (dir = opendir(path)) != NULL) {
        while((entry = readdir(dir)) != NULL)
            if(entry->d_type == DT_DIR && entry->d_name[0] != '.')
                ++count;
        closedir(dir);

        if(count == 0)
            goto skip_pacman;

        snprintf(buf, sizeof(buf), "%u%s", count, _pkg_mgr ? " (pacman)" : "");

        res->string = strdup(buf);

        if(res->string != NULL)
            done = true;
    }
skip_pacman:;

    path[0] = 0;
    if(prefix)
        safeStrncpy(path, prefix, sizeof(path));
    strncat(path, "/var/lib/emerald/db", sizeof(path) - strlen(path) - 1);
    if(_pkg_emerald && (dir = opendir(path)) != NULL) {
        count = 0;
        while((entry = readdir(dir)) != NULL)
            if(entry->d_name[0] != '.')
                ++count;
        closedir(dir);

        if(count == 0)
            goto skip_emerald;

        snprintf(buf, sizeof(buf), "%s%u%s", done ? ", " : "", count, _pkg_mgr ? " (emerald)" : "");
        done = true;

        char *tmpptr = realloc(res->string, (res->string ? strlen(res->string) : 0) + strlen(buf) + 1);
        if(tmpptr == NULL)
            goto skip_emerald;

        if(res->string == NULL)
            tmpptr[0] = 0;

        res->string = tmpptr;
        strcat(res->string, buf);
    }
skip_emerald:;

    path[0] = 0;
    if(prefix)
        safeStrncpy(path, prefix, sizeof(path));
    strncat(path, "/var/lib/dpkg/status", sizeof(buf) - strlen(path) - 1);
    if(_pkg_dpkg && (fp = fopen(path, "r")) != NULL) {
        char line[sizeof(buf) * 2];
        int count = 0;

        while(fgets(line, sizeof(line), fp)) {
            // check if the line starts with "Package:"
            if(strncmp(line, "Package:", 8) == 0)
                ++count;
        }
        fclose(fp);

        if(count == 0)
            goto skip_dpkg;

        snprintf(buf, sizeof(buf), "%s%u%s", done ? ", " : "", count, _pkg_mgr ? " (dpkg)" : "");
        done = true;

        char *tmpptr = realloc(res->string, (res->string ? strlen(res->string) : 0) + strlen(buf) + 1);
        if(tmpptr == NULL)
            goto skip_dpkg;

        if(res->string == NULL)
            tmpptr[0] = 0;

        res->string = tmpptr;
        strcat(res->string, buf);
    }
skip_dpkg:;

#ifdef SQLITE3_EXISTS
    path[0] = 0;
    if(prefix)
        safeStrncpy(path, prefix, sizeof(path));
    strncat(path, "/var/lib/rpm/rpmdb.sqlite", sizeof(path) - strlen(path) - 1);
    if(access(path, F_OK) != 0) {
        path[0] = 0;
        if(prefix)
            safeStrncpy(path, prefix, sizeof(buf));
        strncat(path, "/usr/lib/sysimage/rpm/rpmdb.sqlite", sizeof(path) - strlen(path) - 1);
    }
    if(_pkg_rpm && access(path, F_OK) == 0) {
        sqlite3 *db;
        sqlite3_stmt *stmt;

        char uri[320];
        snprintf(uri, sizeof(uri), "file:%s?immutable=1", path);

        if(sqlite3_open_v2(uri, &db, SQLITE_OPEN_READONLY | SQLITE_OPEN_URI, NULL) != SQLITE_OK)
            goto skip_rpm_c;
        if(sqlite3_prepare_v2(db, "SELECT count(*) FROM Packages", -1, &stmt, NULL) != SQLITE_OK) {
            sqlite3_close(db);
            goto skip_rpm_b;
        }

        if(sqlite3_step(stmt) != SQLITE_ROW)
            goto skip_rpm;

        int count = sqlite3_column_int(stmt, 0);

        if(count <= 0)
            goto skip_rpm;

        snprintf(buf, sizeof(buf), "%s%d%s", done ? ", " : "", count, _pkg_mgr ? " (rpm)" : "");
        done = true;

        char *tmpptr = realloc(res->string, (res->string ? strlen(res->string) : 0) + strlen(buf) + 1);
        if(tmpptr == NULL)
            goto skip_rpm;

        if(res->string == NULL)
            tmpptr[0] = 0;

        res->string = tmpptr;
        strcat(res->string, buf);

    skip_rpm:
        sqlite3_finalize(stmt);
    skip_rpm_b:
        sqlite3_close(db);
    }
skip_rpm_c:
#endif

    path[0] = 0;
    if(prefix)
        safeStrncpy(path, prefix, sizeof(path));
    strncat(path, "/var/lib/flatpak/runtime", sizeof(path) - strlen(path) - 1);
    if(_pkg_flatpak && (dir = opendir(path)) != NULL) {
        count = 0;
        while((entry = readdir(dir)) != NULL)
            if(entry->d_type == DT_DIR && entry->d_name[0] != '.')
                ++count;
        closedir(dir);

        if(count == 0)
            goto skip_flatpak;

        snprintf(buf, sizeof(buf), "%s%u%s", done ? ", " : "", count, _pkg_mgr ? " (flatpak)" : "");
        done = true;

        char *tmpptr = realloc(res->string, (res->string ? strlen(res->string) : 0) + strlen(buf) + 1);
        if(tmpptr == NULL)
            goto skip_flatpak;

        if(res->string == NULL)
            tmpptr[0] = 0;

        res->string = tmpptr;
        strcat(res->string, buf);
    }
skip_flatpak:;

    path[0] = 0;
    if(prefix)
        safeStrncpy(path, prefix, sizeof(path));
    strncat(path, "/var/snap", sizeof(path) - strlen(path) - 1);
    if(_pkg_snap && (dir = opendir(path)) != NULL) {
        count = 0;
        while((entry = readdir(dir)) != NULL)
            if(entry->d_type == DT_DIR && entry->d_name[0] != '.')
                ++count;
        closedir(dir);

        if(count == 0)
            goto skip_snap;
        snprintf(buf, sizeof(buf), "%s%u%s", done ? ", " : "", count, _pkg_mgr ? " (snap)" : "");
        done = true;

        char *tmpptr = realloc(res->string, (res->string ? strlen(res->string) : 0) + strlen(buf) + 1);
        if(tmpptr == NULL)
            goto skip_snap;

        if(res->string == NULL)
            tmpptr[0] = 0;

        res->string = tmpptr;
        strcat(res->string, buf);
    }
skip_snap:;
#endif

    if(_pkg_brew && binaryInPath("brew")) {
        char *args[] = {"brew", "--cellar", NULL};
        execCmd(buf2, 64, args);

        if(buf2[0] == '\0')
            goto skip_brew;

        if((dir = opendir(buf2)) == NULL)
            goto skip_brew;

        count = 0;

        while((entry = readdir(dir)) != NULL)
            if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                ++count;
        closedir(dir);

        if(count == 0)
            goto skip_brew;

        snprintf(buf, sizeof(buf), "%s%u%s", done ? ", " : "", count, _pkg_mgr ? " (brew)" : "");
        done = true;

        char *tmpptr = realloc(res->string, (res->string ? strlen(res->string) : 0) + strlen(buf) + 1);
        if(tmpptr == NULL)
            goto skip_brew;

        if(res->string == NULL)
            tmpptr[0] = 0;

        res->string = tmpptr;
        strcat(res->string, buf);
    }
skip_brew:;

    if(_pkg_pip && binaryInPath("pip")) {
        char *args[] = {"sh", "-c", "pip list 2>/dev/null | wc -l", NULL};
        execCmd(buf2, 16, args);

        if(buf2[0] == '0')
            goto skip_pip;

        snprintf(buf, sizeof(buf) - strlen(buf), "%s%d%s", done ? ", " : "", atoi(buf2) - 2, _pkg_mgr ? " (pip)" : "");
        done = true;

        char *tmpptr = realloc(res->string, (res->string ? strlen(res->string) : 0) + strlen(buf) + 1);
        if(tmpptr == NULL)
            goto skip_pip;

        if(res->string == NULL)
            tmpptr[0] = 0;

        res->string = tmpptr;
        strcat(res->string, buf);
    }
skip_pip:;

    if(!done)
        res->retval = ERR_NO_INFO;
    return res;
}
