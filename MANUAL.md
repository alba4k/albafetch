# Return codes and errors
| Return Code   | Meaning               |
| ---           | ---                   |
| 0             | Correct execution     |
| 1             | Bad argument usage    |
| -1            | Bad `/etc/os-release` |

Note: error -1 can be "fixed" by working on the mentioned file or setting a value for `DEFAULT_LOGO` in `src/config.h`

# Adding new modules
`src/main.c`, in function main():
Those are all the currently supported modules:
```
    void (*infos[])() = {
        separator,
        title,
        hostname,
        user,
        uptime,
        os,
        kernel,
        desktop,
        shell,
        term,
        packages,
        host,
        bios,
        cpu,
        gpu,
        memory,
        public_ip,
        local_ip,
    };
```
Change the order the infos is printed in or add/remove some in your main.c file
## You will need to recompile (and eventually reinstall) after