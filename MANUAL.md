# Return codes and errors
| Return Code   | Meaning               |
| ---           | ---                   |
| 0             | Correct execution     |
| 1             | Bad argument usage    |
| -1            | Bad `/etc/os-release` |

# Adding new modules
`src/main.c`, in function main():
```
    void (*infos[])() = {
        title,
        separator,
        uptime,
        separator, 
        os,
        kernel,
        desktop, 
        shell,
        term,
        packages, 
        separator,
        host,
        cpu,
        gpu,
        memory,
    };
```
Change the order the infos is printed in or add more