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

## Build dependencies
These will also install the relative runtime dependencies

* libcurl
	- On Arch Linux, [libcurl-gnutls](https://archlinux.org/packages/core/x86_64/libcurl-gnutls)
	- On Debian, [libcurl4-gnutls-dev](https://packages.debian.org/stretch/libcurl4-gnutls-dev)
	- On fedora, [libcurl](https://packages.fedoraproject.org/pkgs/curl/libcurl)
	- In Termux, libcurl
* libcurl
	- On Arch Linux, [pciutils](https://archlinux.org/packages/core/x86_64/pciutils)
	- On Debian, [libpci-dev](https://packages.debian.org/buster/libpci-dev)
	- On Fedora, [pciutils-libs](https://packages.fedoraproject.org/pkgs/pciutils/pciutils-libs)
* A build system
	- Make and meson are already set up, more details are found [here](#compilation)

## Runtime dependencies
I would like to eventually remove those, by checking at runtime if they are installed and not use them if not so.
Also, in case albafetch was unable to get the info using those libraries, it'll fall back to `curl` and `lspci` (as system shell commands).

* libcurl (for dynamically linked binaries)
	- On Arch Linux, [libcurl-gnutls](https://archlinux.org/packages/core/x86_64/libcurl-gnutls)	
	- On Debian, [libcurl4](https://packages.debian.org/buster/libcurl4)
	- On Fedora, [libcurl](https://packages.fedoraproject.org/pkgs/curl/libcurl)
	- In Termux, libcurl
* pcilib (for dynamically linked binaries)
	- On Arch Linux, [pciutils](https://archlinux.org/packages/core/x86_64/pciutils)
	- On Debian, [libpci3](https://packages.debian.org/buster/libpci3)
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
There are three packages on the AUR that provide albafetch:
* [albafetch](https://aur.archlinux.org/packages/albafetch-) will compile the source code of the latest release
* [albafetch-bin](https://aur.archlinux.org/packages/albafetch-bin) will install a pre-compiled binary from the latest release
* [albafetch-git](https://aur.archlinux.org/packages/albafetch-git) will compile the source of the latest commit in master

You can find more information on how to install packages from the AUR in the [Arch Wiki](https://wiki.archlinux.org/title/Arch_User_Repository#Installing_and_upgrading_packages)

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

What if your OS is not included in the ones mentioned?
In this case, you can either compile the source code yourself and install albafetch manually, or you can grab an executable from the [latest release](https://github.com/alba4k/albafetch/releases/latest).

Please note that albafetch currently won't run on Windows (despite `albafetch --logo windows` being an option), but I'm planning to eventually add support (sooner or later). Feel free to help :)

```
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch

# make install
```

`make install` needs elevated privileges on Linux (e.g. `sudo` or a root shell) to write to `/usr/bin`, while `/usr/local/bin` can be accessed as a normal user on macOS.

Alternatively, you may prefer meson to do this:

```
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch
$ meson setup build
$ meson compile -C build
$ meson install -C build
```

Meson will install the executable to `/usr/local/bin`, which you may or may not want (executables in this directory are ran instead of ones in `/usr/bin`).

# Contributing

Almost everything included in this program is written in C.

If you want to, you can directly modify the source code contained in this repository and recompile the program afterwards to get some features you might want or need.

New logos can be added in [`src/logos.h`](src/logos.h) (be careful to follow the format), new infos in [`src/info.c`](src/info.c) and [`src/info.h`](src/info.h). You will also need to edit [`src/main.c`](src/main.c) afterwards to fully enable the new features.

Don't mind opening a pull request if you think some of the changes you made should be in the public version, just try to follow the coding style that I used in the rest of the project.

Any contribution, even just a fix of a typo, is highly appreciated.

---

###### © Aaron Blasko

###### Initially started in March 2022
