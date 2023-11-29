# Changes since v4.1

## New Features

### Modules

### Config options
* any invalid escape sequence like "\X" will now just be reduced to "X"
* it is now possible to include "#" and ";" in your config without them counting as a comment by escaping them with a "\\"

### Command line arguments

### Other changes
* the number of lines a custom ascii art may have was reduced to 39 (from 47) but the maximum lenght of each line got increased to 256B (~66% increase in total size)

## Bug fixes

### Noticeable fixes
* Any portion that might have shown white as `\e[37m` will now use `\e[97m`

### Technical fixes
* Reduced the memory usage when not using --ascii (increased when it is used)

---

##### © Aaron Blasko
