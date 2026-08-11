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

## Bug fixes

### Noticeable fixes
* Skip `Default string-WCP` vendor version
* Fixed `rpm` package counts

### Technical fixes
* Improved fread() usage

## Dependencies
* dropped `libpci`
* added `vulkan`

---

##### © Aaron Blasko
