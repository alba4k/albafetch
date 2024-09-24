# albafetch by alba4k

![Intro](images/albafetch.png)

**albafetch** is a lightweight and fast tool designed to display system information in a [neofetch](https://github.com/dylanaraps/neofetch)-like format, but with significantly better performance. It was developed as both a personal challenge and to address neofetch’s slowness, which is understandable given its 10k+ lines of shell script.

> **Important Note**: To avoid merge conflicts, please submit your pull requests to the `development` branch. Ensure you check out this branch before making any changes. The `master` branch is more stable and is updated less frequently, primarily when major features or fixes have been thoroughly tested.

---

## 📚 Table of Contents

1. [Introduction](#introduction)
2. [Dependencies](#dependencies)
   - [Build Dependencies](#build-dependencies)
   - [Runtime Dependencies](#runtime-dependencies)
3. [Compilation](#compilation)
   - [Using Makefile](#using-the-makefile)
   - [Using Meson](#using-meson)
   - [Using Nix](#using-nix)
4. [Installation](#installation)
   - [Arch Linux](#for-arch-linux)
   - [NixOS](#for-nixos)
   - [Manual Installation](#manual-installation)
5. [Configuration](#configuration)
6. [Contributing](#contributing)

---

## 🚀 Introduction

**albafetch** provides detailed system information in under a second, making it much faster than comparable tools. Below is a preview of the tool's default appearance and a comparison of its execution time versus neofetch.

<details>
  <summary>Preview</summary>
  This is the default look of albafetch:

  ![Default](images/albafetch_demo_default.png)

  Here's what it looks like with my custom configuration:
  
  ![Custom](images/albafetch_demo.png)
</details>

<details>
  <summary>Time Comparison</summary>

  Neofetch:
  ![Neofetch](images/time_neofetch.png)

  Albafetch:
  ![Albafetch](images/time_albafetch.png)

</details>

The tool currently supports various GNU/Linux distributions, macOS (both x64 and arm64), and Android (tested on Termux). You’re welcome to test it on other platforms.

You can find more detailed usage and configuration information in the [User Manual](MANUAL.md), and recent changes in the [Changelog](CHANGELOG.md).

---

## 🛠️ Dependencies

### Build Dependencies

The following packages are needed to build **albafetch**, which will also include necessary runtime dependencies:

- **libpci**:
  - Arch Linux: [pciutils](https://archlinux.org/packages/core/x86_64/pciutils)
  - Debian: [libpci-dev](https://packages.debian.org/buster/libpci-dev)
  - Fedora: [pciutils-devel](https://packages.fedoraproject.org/pkgs/pciutils/pciutils-devel)
  - Alpine Linux: [pciutils-dev](https://pkgs.alpinelinux.org/package/edge/main/x86_64/pciutils-dev)

- **libc** (likely already installed):
  - Alpine Linux: [musl-dev](https://pkgs.alpinelinux.org/package/edge/main/x86_64/musl-dev)

- **Build systems**: 
  - **Make** and **Meson** are supported. For more details, see the [compilation section](#compilation).

### Runtime Dependencies

These dependencies are required at runtime, but **albafetch** may function without them by checking their presence dynamically.

- **libpci** for dynamically linked binaries:
  - Arch Linux: [pciutils](https://archlinux.org/packages/core/x86_64/pciutils)
  - Debian: [libpci3](https://packages.debian.org/buster/libpci3)
  - Fedora: [pciutils-libs](https://packages.fedoraproject.org/pkgs/pciutils/pciutils-libs)
  
- A `sh` binary must be available in your system’s `PATH`. This is generally fulfilled on all UNIX-like systems.

---

## ⚙️ Compilation

### Using the Makefile

Ensure you have a C compiler installed. **albafetch** uses Meson under the hood, even when using the Makefile.

```bash
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch
$ make
```

If the compilation succeeds, the executable will appear as `build/albafetch`.

#### Debug Builds

To build a debug binary (`build/debug`), which tests all functions, run:

```bash
$ make debug
```

### Using Meson

For those who prefer Meson and Ninja:

```bash
$ meson setup build
$ meson compile -C build
$ build/albafetch
```

### Using Nix

With Nix, compiling becomes simpler, especially for static builds or cross-compilation. Some preset outputs are available:

```bash
$ nix build .#albafetch  # regular dynamically linked build
$ nix build .#albafetch-static  # statically linked build (Linux only)
$ nix build .#albafetch-arm  # cross-compile from x86_64 to ARM (x86_64 only)
```

---

## 📦 Installation

### For Arch Linux

You can install **albafetch** from the Arch User Repository (AUR). Three AUR packages are available:

- [albafetch](https://aur.archlinux.org/packages/albafetch) – builds the latest release.
- [albafetch-bin](https://aur.archlinux.org/packages/albafetch-bin) – installs pre-built binaries.
- [albafetch-git](https://aur.archlinux.org/packages/albafetch-git) – builds the latest commit from `master`.

Refer to the [Arch Wiki](https://wiki.archlinux.org/title/Arch_User_Repository#Installing_and_upgrading_packages) for instructions on installing AUR packages.

### For NixOS

Install **albafetch** using `nix profile` or `nix-env`:

```bash
$ nix profile install .#albafetch
$ nix-env -iA packages.<platform>.albafetch
```

For Flake-based installations, see the provided example in the `README`.

### Manual Installation

If your OS is not covered, you can manually compile and install **albafetch**:

```bash
$ git clone https://github.com/alba4k/albafetch
$ cd albafetch
$ make
$ sudo make install
```

Alternatively, you can use Meson for installation:

```bash
$ meson setup build
$ meson compile -C build
$ meson install -C build
```

Meson installs to `/usr/local/bin`, whereas Make installs to `/usr/bin`.

---

## ⚙️ Configuration

You can customize **albafetch** through a configuration file, typically located at `~/.config/albafetch.conf` or `/etc/xdg/albafetch.conf`. An example config file is available [here](albafetch.conf).

For more detailed explanations of configuration options, consult the [User Manual](MANUAL.md).

---

## 🤝 Contributing

Contributions are welcome! The project is mainly written in C, and new features, improvements, or bug fixes are appreciated.

To contribute:

1. Fork the repository.
2. Make your changes in the appropriate files:
   - Add new logos in [`src/logos.h`](src/logos.h).
   - Modify system information retrieval in [`src/info/`](src/info).
   - Config parsing is handled in [`src/utils.c`](src/utils.c).
3. Submit a pull request against the `development` branch.

All contributions, whether new features or typo corrections, are valued.

---

###### © 2024 Aaron Blasko