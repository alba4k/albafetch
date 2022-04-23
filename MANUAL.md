# Return codes and errors
| Return Code   | Meaning               |
| ---           | ---                   |
| 0             | Correct execution     |
| 1             | Bad argument usage    |
| -1            | Bad `/etc/os-release` |

# Adding new modules
`src/main.c`, in function main():
```
line = printLogo(line);
module();
```
Only change the module() function call with the desired one