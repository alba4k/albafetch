## New Features

### Command line arguments
* `--no-config`: Ignores any provided or existing config file

### Config options
* `date_format`: Specifies how the date should be formatted
* `term_ssh`: prints `(SSH)` after the terminal name when running inside of an SSH connection


## Bug fixes

### Noticeable fixes
* Fixed a bug where the separators would print of the wrong lenght because of logo escape sequences parsed incorrectly

### Technical fixes
* The memory module can now print in up to 256B (was 200B because of 55 reserved for the percentage)
* Reduced the amount of memory the boolean options take (by bitmasking a big 64-bit integer)

---

###### © Aaron Blasko
