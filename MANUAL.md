# Return codes and errors
| Return Code   | Meaning           |
|---            |---                |
| 0             | Correct execution |
| 1             | Error in --color  |
| 2             | Error in --bold   |
| 3             | Error in --logo   |

# Adding new modules
`src/albafetch.c`, in function main():
```
printf("%s%s%s" SPACING, color, bold, logo[line]);
module();
line++;
```
Only change the module() function call with the desired one