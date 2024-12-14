# Changes since v4.2.1

## New Features

### Modules

### Config syntax
* Custom ascii arts will now print in the specified color by default

### Command line arguments
* Added `--version` (`-v` for short)

### Other changes
* added a logo for [Rocky Linux](https://rockylinux.org)
* updated the [Garuda Linux](https://garudalinux.org) logo

## Bug fixes

### Noticeable fixes
* Packages should be counted faster for dpkg and especially snap

### Technical fixes
* Reduced the size of default logos
* Moved some functions from `src/utils.c` to `src/config/config.c` and `src/config/config.c`.
  Planning on rewriting config parsing and reducing `src/utils.c` even more.
* Memory safety improvements in packages

## Dependencies
* sqlite3 is now needed to build the project
* meson, ninja and pkg-config are now used in the Makefile
* glib is now an optional dependency. If not installed at compile time, custom implementations will be used
* libpci is not an optional dependency at compile time too

---

##### © Aaron Blasko
