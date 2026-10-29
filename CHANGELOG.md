# Changes since v4.3

## New Features

### Modules

### Config syntax

### Command line arguments

### Other
* Added logo for Fedora Asahi Remix

## Changes
* `gpu` now uses vulkan to find the exact gpu model (not libpci) except on macOS
* `cpu` now prints `Core` (e.g. in `Intel Core i7-1260P`) even when using `cpu_brand = "false"`
* `memory` now only prints `MiB` at the end (e.g. `X / Y MiB` instead of `X MiB / Y MiB`)
* `icon_theme`, `gtk_theme` and `cursor_theme` should now be considerably faster

## Bug fixes

### Noticeable fixes
* Skip `Default string-WCP` vendor version
* Fixed `rpm` package counts
* First line of custom ASCII was skipped when not a color

### Technical fixes
* Improved fread() usage
* Rewrote info gathering structure:
    - should slightly reduce memory consumption
    - easier future parallelization? no more resource sharing
* Memory safety improvements and refactoring

## Dependencies
* dropped `libpci`
* dropped `glibc`
* added `vulkan`
* building without `sqlite3` and `vulkan` now disables the features they're needed for

---

##### © Aaron Blasko
