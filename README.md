# albafetch ~by alba4k

![intro](images/albafetch.png)

albafetch is a simple, fast system fetching program. It prints many info about the system in way less than a second. I decided to make this as a challenge for myself and since I found neofetch too slow (which is understandable from a 10k+ lines shell script).

Here is a time comparison (exact execution times change between machines and runs):

![neofetch](images/time_neofetch.png)
![albafetch](images/time_albafetch.png)

## Dependencies:
* sh (packages)
* wc (packages)

All of those will be present by default on a normal GNU/Linux installation.

# Compiling:
```shell
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch/
$ make
```
An executable file will appear in `albafetch/`

# Installation
### FForor Arch Linux based systems, an AUR package is avaiable (`albafetch-git`)

Manual installation:

```
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch/
# make install
```

`make install` needs to be ran as root (e.g. using `sudo` or in a root shell) to acces `/usr/bin/`. It will compile `albafetch/src/main.c` to this directory.



# Customizing
A configuration file can be found in `~/.config/albafetch.conf`

## How to write the config:
Basic rules:
* comments start with ; and reach the end of the line
* values can't be longer than 32 characters
* only ASCII characters will work

The config should be written in a key "value" format you can put something in between if you like (e.g. 'key = "value"' everything but a " or a keyword

This repository contains an example config file (`./albafetch.conf`)

## Source code editing:
If you like, you can directly modify the source code contained in this repository and recompile the program after. New logos can be added in `albafetch/src/logos.h` (inside of `const char *logos[][]`) and in `./src/main.c` (to make them show up in the help message).

Don't mind doing a pull request if you think some of the changes you made should be in the global version ;)

---

### Aaron Blasko, March 2022
