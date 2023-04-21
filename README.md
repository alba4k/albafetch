# albafetch ~by alba4k

![intro](images/albafetch.png)

albafetch is a simple and fast program to display a lot of system information in a neofetch-like layout in way less than a second. I decided to make this as a challenge for myself and since I found neofetch too slow (which is understandable given that we're talking about a 10k+ lines shell script).

Here is a time comparison (exact execution times change between machines and runs):

![neofetch](images/time_neofetch.png)
![albafetch](images/time_albafetch.png)

For additional usage-related info, check [`MANUAL.md`](MANUAL.md).

## Table of contents
1. [Dependencies](#dependencies)
	* [For building](#build-dependencies)
	* [At runtime](#runtime-dependencies)
2. [Compilation](#compilation)
	* [make](#using-the-makefile)
	* [meson](#using-meson)
	* [nix](#using-nix)
3. [Installation](#installation)
	* [Arch BTW](#for-arch-linux)
	* [NixOS](#for-nixos)
	* [Manually](#manual-installation)
4. [Contributing](#contributing)
	


# Dependencies

the only dependencies are `pcilib` ([pciutils on arch](https://archlinux.org/packages/core/x86_64/pciutils), [libpci3 on arch](https://packages.debian.org/buster/libdevel/libpci3)) and `libcurl` ([lib32-curl on arch](https://archlinux.org/packages/core/x86_64/libcurl-gnutls), [libpci-dev on arch](https://packages.debian.org/buster/libdevel/libpci-dev)). Also, `lspci` and `curl` are used as fallback in case those libraries were unable to get the necessary info.

## Build dependencies
These will also install the relative runtime dependencies

* libcurl
	- On Arch Linux, [libcurl-gnutls](https://archlinux.org/packages/core/x86_64/libcurl-gnutls)
	- On Debian, [libcurl4-gnutls-dev](https://packages.debian.org/stretch/libcurl4-gnutls-dev)
	- On fedora, [libcurl](https://packages.fedoraproject.org/pkgs/curl/libcurl)
	- In Termux, libcurl
* libcurl
	- On Arch Linux, [pciutils](https://archlinux.org/packages/core/x86_64/pciutils)
	- On Debian, [pcilib-dev](https://packages.debian.org/buster/libpci-dev)
	- On Fedora, [pciutils-libs](https://packages.fedoraproject.org/pkgs/pciutils/pciutils-libs)
* A build system
	- Make and meson are already set up, more details are found [here](#compilation)

## Runtime dependencies
I would like to eventually remove those, by checking at runtime if they are installed and not use them if not so.

* libcurl (for dynamically linked binaries)
	- On Arch Linux, [libcurl-gnutls](https://archlinux.org/packages/core/x86_64/libcurl-gnutls)	
	- On Debian, [libcurl4](https://packages.debian.org/buster/libcurl4)
	- On Fedora, [libcurl](https://packages.fedoraproject.org/pkgs/curl/libcurl)
	- In Termux, libcurl
* pcilib (for dynamically linked binaries)
	- On Arch Linux, [pciutils](https://archlinux.org/packages/core/x86_64/pciutils)
	- On Debian, [pcilib-dev](https://packages.debian.org/buster/libpci3)
	- On Fedora, [pciutils-libs](https://packages.fedoraproject.org/pkgs/pciutils/pciutils-libs)
* there must be a `sh` binary in your PATH. This should already be satisfied on any UNIX-like system

# Compilation

## Using the Makefile

This will need gcc (`make CC=[compiler]` for other compilers, which should accept the same flags as gcc, e.g. clang) and make

```shell
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch
$ make
```

An executable file should appear as `build/albafetch` if the compilation succeeds.

### Debug builds
It is possible to build a debug binary (`build/debug`) that will test every single function and make sure it runs correctly. This can be done by running

```sh
$ make debug
```

## Using meson

If you prefer to build with meson/ninja, you can use these commands:

```sh
meson setup build
meson compile -C build
```

Like `make`, an executable file with appear in `build/` if compilation succeeds

## Using nix

Building with nix can make compiling in some ways much easier, such as when compiling statically
or cross compilng. A few convenience outputs are included:

```sh
nix build .#albafetch # regular, dynamically linked build
nix build .#albafetch-static # statically linked build
nix build .#arm.<linux/darwin> # cross compilng from x86_64 to arm
```

# Installation

## For Arch Linux
An AUR package is available, [albafetch-git](https://aur.archlinux.org/packages/albafetch-git).
This can be installed using:
```sh
$ git clone https://aur.archlinux.org/albafetch-git.git
$ cd albafetch-git
$ makepkg -si
```
or using an AUR helper.

## For NixOS

`nix profile`:

```sh
$ nix profile install .#albafetch
```

`nix shell`:

```sh
$ nix shell github:alba4k/albafetch
```

`nix-env`:

```sh
$ nix-env -iA packages.<your platform>.albafetch # platform examples: x86_64-linux, aarch64-linux, aarch64-darwin
```

As an overlay:

```nix
{
	albafetch,
	pkgs,
	...
}: {
	nixpkgs.overlays = [albafetch.overlays.default];
	environment.systemPackages = with pkgs; [
		albafetch
	];
}
```

## Manual installation

This will need gcc (`make CC=[compiler] install` for other compilers, which should accept the same flags as gcc, e.g. clang) and make

```
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch

# make install
```

`make install` needs elevated privileges (e.g. `sudo` or a root shell) to write to `/usr/bin` or `/usr/local/bin` (macOS).

Alternatively, you may use meson to do this:

```
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch
$ meson setup build
$ meson compile -C build
$ cd build
$ meson install
```

Please note that meson will always install the executable to `/usr/local/bin`, which you may or may not want (executables in this directory are ran instead of ones in `/usr/bin`)

# Contributing

Almost everything included in this program is written in C.

If you want to, you can directly modify the source code contained in this repository and recompile the program afterwards to get some features you might want or need.

New logos can be added in [`src/logos.h`](src/logos.h) (be careful to follow the format), new infos in [`src/info.c`](src/info.c) and [`src/info.h`](src/info.h). You will also need to edit [`src/main.c`](src/main.c) afterwards to fully enable the new features.

Don't mind opening a pull request if you think some of the changes you made should be in the public version, just try to follow the coding style that I used in the rest of the project.

Any contribution, even just a fix of a typo, is highly appreciated.

---

### © Aaron Blasko

###### Initially started in March 2022
