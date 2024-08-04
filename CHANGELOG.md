# Changes since v4.1

## New Features

### Modules
* `icon_theme`: prints the current icon theme for GTK
* `cursor_theme`: prints the current cursor theme for GTK

### Config syntax
* any invalid escape sequence like "\X" will now just be reduced to "X"
* it is now possible to include "#" and ";" in your config without them counting as a comment, as long as they are enclosed between two `"` (which should always be true anyway)
* `col_block_len` was removed, in favor of `col_block_str`, which contains the entire string that is printed for each color
* `col_background` was added, which controls whether the background or the background of `col_block_str` is colored (vs. the foreground)

### Command line arguments

### Other changes
* the number of lines a custom ascii art may have was reduced to 40 (from 47) but the maximum length of each line got increased to 256B (~70% increase in total size)
* the buffer size for single lines has been raised to 1024B
* the debug script can now be used with `--no-pip`, this won't print the # of packages installed using pip (because of speed concerns)
* the cpu architecture will now be printed in brackets (eg: `Arch Linux x86_64` -> `Arch Linux (x86_64)`)
* using `--logo none` will prevent any logo from being displayed
* added a logo for [Mageia](https://mageia.org)
* added a logo for [NixOS](https://nixos.org)
* Terminator will now be shown as terminal
* Trying to use TERM_PROGRAM to find the terminal name

## Bug fixes

### Noticeable fixes
* Any portion that might have shown white as `\e[37m` will now use `\e[97m`
* The amount of packages installed using rpm should now be much faster
* Logo wuoldn't print correctly on EndeavourOS and Asahi Fedora
* not printing "None" as model version (VMware VMs)

### Technical fixes
* Reduced the memory usage when not using --ascii (increased when it is used)
* `src/info.c` was split up into multiple files (in `src/info/`)
* libcurl is now not used anymore to get the public ip
* the length of logos is calculated on the fly, resulting in lower memory usage

---

##### © Aaron Blasko
