# Changes since v4.2.1

## New Features

### Modules
* Added a `swap` module that prints the used and total swap on linux
* The `cpu` modules now prints 2 decimal digits is the frequency is below 1 GHz

### Config syntax
* Custom ascii arts will now print in the specified color by default
* Added `swap_label`, the label for the new swap module
* Added `swap_perc`, whether the used swap percentage should be printed

### Command line arguments
* Added `--version` (`-v` for short), prints version and build commit

### Other
* Added a logo for [Rocky Linux](https://rockylinux.org)

## Changes
* Updated the [Garuda Linux](https://garudalinux.org) logo
* Changed return values to be more clear than 0 or 1
* User errors are now handled gracefully

## Bug fixes

### Noticeable fixes
* Packages should be counted faster for dpkg and especially snap
* Alacritty will now be properly recognized even with `general.ipc_socket = false`
* `gpu` should now print all gpus even when libpci doesn't work
* Fixed `pwd_path` not working
* Should now exit on OOM, not segfault

### Technical fixes
* Reduced the size of default logos
* Moved some functions from `src/utils/utils.c` to `src/config/config.c` and `src/config/config.c`.
  Planning on rewriting config parsing and reducing `src/utils/utils.c` even more.
* Minor memory safety improvements

## Dependencies
* sqlite3 is now needed to build the project
* meson, ninja and pkg-config are now used in the Makefile
* glib is now an optional dependency. If not installed at compile time, custom implementations will be used
* libpci is not an optional dependency at compile time too

---

##### © Aaron Blasko
