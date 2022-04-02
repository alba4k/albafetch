# Return codes and errors
| Return Code   | Meaning                                       |
|---            |---                                            |
| 0             | Correct execution                             |
| 1             | Bad arguments (check ~/.albafetch.log)        |
| 2             | Encountered error 1 + Couldn't open log file  |
| 3             | Couldn't open log file when using --clean-log |

# Adding new modules
`src/albafetch.c`, in function main():
```
printf("%s%s%s" SPACING, color, bold, logo[line]);
module();
line++;
```
Only change the module() function call with the desired one