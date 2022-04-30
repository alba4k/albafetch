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
```
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch/
# make && make install
```
`make install` needs to be ran as root (e.g. using `sudo` or in a root shell) to acces `/usr/bin/`. It will compile `albafetch/src/main.c` to this directory.

# Customizing
```
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch/src/ 
```
You can change some settings in `albafetch/src/config.h`, or by directly modifying the entire source code in `albafetch/src/`. You will need to recompile/reinstall after configuring. Please note that the "logo" entry in `albafech/src/config.h` only changes the defalt logo. New logos can be added in `albafetch/src/vars.h` (as `char* []`) and in `./src/main.c` (to launch with the `--logo` argument).

---

### Aaron Blasko, March 2022
