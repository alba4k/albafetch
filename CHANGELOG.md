# Changes since v4.0

# New Features

### Modules
* `gtk_theme`: Prints the current GTK theme (off by default)

### Config options
* `ascii_art`: Path to a file containing a custom logo (and color)
* `modules`: Module array that specifies which modules should be printed
* `kernel_type`: prints the kernel type in brackets (e.g. `(zen)`)
* `term_ssh`: prints `(SSH)` after the terminal name when running inside of an SSH connection
* `date_format`: Specifies how the date should be formatted
* renamed `loc_localdomain` to `loc_localhost`
* `desktop_type` is now enabled by default

### Command line arguments
* `--ascii`: Path to a file containing a custom logo (and color)
* `--no-config`: Ignores any provided or existing config file
* `--no-logo`: Prevents a logo or ascii art from being printed (while still using it for the color)


# Bug fixes

### Noticeable fixes
* Fixed a bug where the separators would print of the wrong length because of logo escape sequences parsed incorrectly
* Fixed `loc_localhost` (used to be `loc_localdomain`) and `loc_docker` not working
* Title no longer uses bold when `title_color` is set to false
* Up to 3 GPUs will be listed, instead of just the first found
* gpu() will now hide `Rev. A` or similar in NVIDIA cards
* host() will not print `To Be Filled By O.E.M.` anymore

### Technical fixes
* The memory module can now print in up to 256B (was 200B because of 55 reserved for the percentage)
* Reduced the amount of memory the boolean options take (by bit-masking a big 64-bit integer)

---

##### © Aaron Blasko
