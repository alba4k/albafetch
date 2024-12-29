#!/usr/bin/env bash

version=$(../build/albafetch --version | cut -d " " -f 1)
arch=$(uname -m)

if [ "$arch" = "x86_64" ]; then
    arch="amd64"
elif [ "$arch" = "aarch64" ]; then
    arch="arm64"
else
    arch="any"
fi

echo -e "\n\e[1m[\e[33mINFO\e[0m\e[1m]\e[0m Detected version $version"
echo -e "\e[1m[\e[33mINFO\e[0m\e[1m]\e[0m Detected architecture $arch\n"

dirname="albafetch_"$version"_"$arch

mkdir -p "$dirname/DEBIAN"

install -Dm 755 "../build/albafetch" "$dirname/usr/bin/albafetch"

install -Dm 644 "../albafetch.conf" "$dirname/etc/xdg/albafetch.conf"
install -Dm 644 "../LICENSE" "$dirname/usr/share/licences/albafetch/LICENSE"
install -Dm 644 "../README.md" "$dirname/usr/share/doc/albafetch/README.md"
install -Dm 644 "../MANUAL.md" "$dirname/usr/share/doc/albafetch/MANUAL.md"

echo "Package: albafetch
Version: $version
Architecture: $arch
Essential: no
Priority: optional
Depends: libpci3
Maintainer: Aaron Blasko
Description: Faster neofetch alternative, written in C." > "$dirname"/DEBIAN/control

dpkg-deb --build "$dirname"

echo -e -n "\n\e[1m\e[32mInstall Now\e[0m\e[1m [Y/n] ?\e[0m > "
read -r -n 1 install

if [ "$install" = "N" ] || [ "$install" = "n" ]; then
    echo -e "\n\e[1m[\e[32mDONE\e[0m\e[1m]\e[0m File created as $PWD/$dirname.deb"
else
    sudo dpkg -i "$dirname.deb"
fi