# Return codes and errors
| Return Code   | Meaning               |
| ---           | ---                   |
| 0             | Correct execution     |
| 1             | Bad argument usage    |
| -1            | Bad `/etc/os-release` |

Note: error -1 can be "fixed" by working on the mentioned file or setting a value for `default_logo` in your configuration file

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
        current_shell,
        default_shell,
        term,
        packages,
        host,
        bios,
        cpu,
        gpu,
        memory,
        public_ip,
        local_ip,
        pwd,
        colors,
        light_colors
    };
```
Change the order the infos is printed in or add/remove some in your main.c file
## You will need to recompile (and eventually reinstall) after