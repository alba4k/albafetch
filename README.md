# albafetch ~by alba4k

![intro](images/albafetch.png)

albafetch is a simple, fast system fetching program. It prints many info about the system in way less than a second. I decided to make this as a challenge for myself and since I found neofetch too slow (which is understandable from a 10k+ lines shell script).

Here is a time comparison (exact execution times change between machines and runs):

![neofetch](images/time_neofetch.png)
![albafetch](images/time_albafetch.png)

## Dependencies:

Everything the program could need will be installed on your distribution out of te box (`sh`, `lspci`).
I would reccoment double checking libalpm on archlinux-based systems (it's also a dependency of pacman, so that should already be there).

# Compiling:
this will need gcc (`make CC=[compiler]` for other compilers, which should accept the same flags as gcc, e.g. clang) and make
```shell
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch
$ make
```
An executable file should appear in `build/` if the compilation succedes.

# Installation
### For Arch Linux based systems, an AUR package is avaiable (`albafetch-git`)
## Manual installation:

this will need gcc (`make CC=[compiler] install` for other compilers, which should accept the same flags as gcc, e.g. clang) and make

```
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch
# make install
```

`make install` needs to be ran as root (e.g. using `sudo` or in a root shell) to acces `/usr/bin/`. It will copy the executable from `build/` to there.



# Customizing
A configuration should be placed in `~/.config/albafetch.conf` (`~/.config` can be overwritten using the `XDG_CONFIG_HOME` environnment variable). An example config, with comments, is found in this repository.

## How to write the config:
Basic rules:
* comments start with ; and reach the end of the line
* values can't be big than 32B (separators can get as long as 64B)
* only ASCII characters will work
* ending the file with a newline is reccomended

The config should be written in a key "value" format you can put something in between if you like (e.g. 'key = "value"' everything but a " or a keyword.

### Options and default values:
``` python
spacing = "    "
separator1 = "\e[0m------------------"
separator2 = ""
separator_first = "\e[0m------------------"
separator_last = "\e[0m------------------"
dash_str = "\e[0m:"

print_cpu_freq = "true"
print_cpu_brand = "true"
print_gpu_freq = "true"
print_mem_perc = "true"
align_infos = "false"

default_color = ""
default_bold = "true"
default_logo = ""

col_block_len = "3"

title_prefix = ""
col_prefix = ""
hostname_label = "Hostname"
user_label = "User"
uptime_label = "Uptime"
os_label = "OS"
kernel_label = "Kernel"
desktop_label = "Desktop"
shell_label = "Shell"
term_label = "Terminal"
packages_label = "Packages"
host_label = "Host"
bios_label = "BIOS"
cpu_label = "CPU"
gpu_label = "GPU"
mem_label = "Memory"
pub_ip_label = "Publ. IP"
loc_ip_label = "Priv. IP"
pwd_label = "Directory"
```

Unset values will use the defaults
Quotes in the config are **not** optional.

If a key is defined more than one the first one is the only oen that will get considered.

This repository contains a commented example config file (`albafetch.conf`) with the default values.

## Source code editing:
If you like, you can directly modify the source code contained in this repository and recompile the program after. New logos can be added in `src/logos.h` (inside of `const char *logos[][]`) and in `src/main.c` (to make them show up in the help message).

Don't mind doing a pull request if you think some of the changes you made should be in the global version ;)§

---

### Aaron Blasko, March 2022
