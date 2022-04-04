# Return codes and errors
| Return Code   | Meaning               |
|---            |---                    |
| 0             | Correct execution     |
| 1             | Bad argument usage    |

# Adding new modules
`src/albafetch.c`, in function main():
```
printf("%s%s%s" SPACING, color, bold, logo[line]);
module();
line++;
```
Only change the module() function call with the desired one