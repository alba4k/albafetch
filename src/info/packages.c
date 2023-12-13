#include "info.h"
#include "../utils.h"

#include <string.h>

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
    int pipes[2];
    bool done = false;

    #ifndef __APPLE__   // package managers that won't run on macOS
        FILE *fp;

        path[0] = 0;
        if(getenv("PREFIX"))
            strncpy(path, getenv("PREFIX"), 255);
        strncat(path, "/var/lib/pacman/local", 256-strlen(path));
        if(pkg_pacman && (dir = opendir(path))) {
            while((entry = readdir(dir)) != NULL)
                if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
                    ++count;

            if(count) {
                snprintf(dest, 255 - strlen(buf), "%s%u%s", done ? ", " : "", count, pkg_mgr ? " (pacman)" : "");
                done = true;
            }
            closedir(dir);
        }

        path[0] = 0;
        if(getenv("PREFIX"))
            strncpy(path, getenv("PREFIX"), 255);
        strncat(path, "/var/lib/dpkg/status", 256-strlen(path));
        if(pkg_dpkg && (fp = fopen(path, "r"))) {   // alternatively, I could use "dpkg-query -f L -W" and strlen
            fseek(fp, 0, SEEK_END);
            size_t len = (size_t)ftell(fp);
            rewind(fp);

            char *dpkg_list = malloc(len);
            dpkg_list[fread(dpkg_list, 1, len, fp) - 1] = 0;

            fclose(fp);

            count = 0;
            // this will be wrong if some package does not have "\nInstalled-Size: "
            // or if some package (for some reason) has it in the package description
            while((dpkg_list = strstr(dpkg_list, "\nInstalled-Size: "))) {
                ++count;
                ++dpkg_list;
            }
            free(dpkg_list);

            if(count) {
                snprintf(buf, 256, "%u%s", count, pkg_mgr ? " (dpkg)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }
        }

        path[0] = 0;
        if(getenv("PREFIX"))
            strncpy(path, getenv("PREFIX"), 255);
        strncat(path, "/bin/rpm", 256-strlen(path));
        if(pkg_rpm && !access(path, F_OK)) {
            if(pipe(pipes))
                return 1;

            if(!fork()) {
                close(pipes[0]);
                dup2(pipes[1], STDOUT_FILENO);

                execlp("sh", "sh", "-c", "rpm -qa 2>/dev/null | wc -l", NULL); 
            }
            wait(0);
            close(pipes[1]);
            str[read(pipes[0], str, 16) - 1] = 0;
            close(pipes[0]);

            if(str[0] != '0' && str[0]) {
                snprintf(buf, 255 - strlen(buf), "%s%s%s", done ? ", " : "", str, pkg_mgr ? " (rpm)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }
        }

        path[0] = 0;
        if(getenv("PREFIX"))
            strncpy(path, getenv("PREFIX"), 255);
        strncat(path, "/var/lib/flatpak/runtime", 256-strlen(path));
        if(pkg_flatpak && (dir = opendir(path))) {
            count = 0;
            while((entry = readdir(dir)) != NULL)
                if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
                    ++count;

            if(count) {
                snprintf(buf, 255 - strlen(buf), "%s%u%s", done ? ", " : "", count, pkg_mgr ? " (flatpak)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }
            closedir(dir);
        }

        path[0] = 0;
        if(getenv("PREFIX"))
            strncpy(path, getenv("PREFIX"), 255);
        strncat(path, "/bin/snap", 256-strlen(path));
        if(pkg_snap && !access(path, F_OK)) {
            if(pipe(pipes))
                return 1;

            if(!fork()) {
                close(pipes[0]);
                dup2(pipes[1], STDOUT_FILENO);

                execlp("sh", "sh", "-c", "snap list 2>/dev/null | wc -l", NULL); 
            }

            wait(0);
            close(pipes[1]);
            str[read(pipes[0], str, 16) - 1] = 0;
            close(pipes[0]);

            if(str[0] != '0' && str[0]) {
                snprintf(buf, 255 - strlen(buf), "%s%d%s", done ? ", " : "", atoi(str)-1, pkg_mgr ? " (snap)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }
        }
    #endif
    if(pkg_brew && (!access("/usr/local/bin/brew", F_OK) || !access("/opt/homebrew/bin/brew", F_OK) || !access("/bin/brew", F_OK))) {
        if(pipe(pipes))
            return 1;

        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);
            execlp("brew", "brew", "--cellar", NULL); 
        }

        close(pipes[1]);

        wait(0);

        str[read(pipes[0], str, 128) - 1] = 0;
        close(pipes[0]);

        if(str[0]) {
            if((dir = opendir(str))) {
                count = 0;

                while((entry = readdir(dir)) != NULL)
                    if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
                        ++count;

                if(count) {
                    snprintf(buf, 256, "%s%u%s", done ? ", " : "", count, pkg_mgr ? " (brew)" : "");
                    done = true;
                    strncat(dest, buf, 256 - strlen(dest));
                }

                closedir(dir);
            }
        }
    }


    path[0] = 0;
    if(getenv("PREFIX"))
        strncpy(path, getenv("PREFIX"), 255);
    strncat(path, "/bin/pip", 256-strlen(path));
    if(pkg_pip && !access(path, F_OK)) {
        if(pipe(pipes))
            return 1;

        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("sh", "sh", "-c", "pip list 2>/dev/null | wc -l", NULL); 
        }

        wait(0);
        close(pipes[1]);
        str[read(pipes[0], str, 16) - 1] = 0;
        close(pipes[0]);

        if(str[0] != '0' && str[0]) {
            snprintf(buf, 255 - strlen(buf), "%s%d%s", done ? ", " : "", atoi(str)-2, pkg_mgr ? " (pip)" : "");
            done = true;
            strncat(dest, buf, 256 - strlen(dest));
        }
    }

    return !done;
}
