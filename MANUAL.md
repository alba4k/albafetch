# Return codes and errors
| Return Code   | Meaning               |
|---            |---                    |
| 0             | Correct execution     |
| 1             | Bad argument usage    |

# Adding new modules
`src/main.c`, in function main():
```
printLogo(&line);
module();
```
Only change the module() function call with the desired one