# Changes since v4.1

## New Features

### Modules

### Config syntax
* any invalid escape sequence like "\X" will now just be reduced to "X"
* it is now possible to include "#" and ";" in your config without them counting as a comment by escaping them with a "\\"
* `col_block_len` was removed, in favor of `col_block_str`, which contains the entire string that is printed for each color
* `col_foreground` was added, which controls whether the background or the foreground of `col_block_str` is colored 

### Command line arguments

### Other changes
* the number of lines a custom ascii art may have was reduced to 39 (from 47) but the maximum length of each line got increased to 256B (~66% increase in total size)
* the buffer size for single lines has been raised to 1024B
* the debug script can now be used with `--no-pip`, this won't print the # of packages installed using pip (because of speed concerns)
* the cpu architecture will now be printed in brackets (eg: `Arch Linux x86_64` -> `Arch Linux (x86_64)`)

## Bug fixes

### Noticeable fixes
* Any portion that might have shown white as `\e[37m` will now use `\e[97m`
* The amount of packages installed using rpm should now be much faster

### Technical fixes
* Reduced the memory usage when not using --ascii (increased when it is used)
* `src/info.c` was split up into multiple files (in `src/info/`)
* libcurl is now not used anymore to get the public ip

---

##### © Aaron Blasko
