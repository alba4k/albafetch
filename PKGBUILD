# Maintainer: alba4k <blaskoazzolaaaron@gmail.com>
pkgname=albafetch-git
pkgver=2.5.r.b52d5a9
pkgrel=1
epoch=
pkgdesc="Noefetch, but written in C; both faster and worse than the original"
arch=(x86_64)
url="https://github.com/alba4k/albafetch"
license=('MIT')
groups=()
depends=()
makedepends=('git' 'make' 'gcc')
checkdepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=("git+$url")
noextract=()
md5sums=('SKIP')
validpgpkeys=()

pkgver() {
    cd "${_pkgname}"
    printf "2.5.r%s.%s" "$(git rev-list --cout HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
    cd albafetch
    make
}

package() {
    cd albafetch
    mkdir -p $(pkgdir)/opt/${pkgname}
    cp -rf * ${pkgdir}/opt/${pkgname}
    make DESTDIR="${pkgdir}" PREFIX="/usr" install
    install -Dm644 LICENSE "${pkgdir}/usr/share/licenses/${pkgname}/LICENSE"
    install -Dm644 README.md "${pkgdir}/usr/share/doc/${pkgname}/README.md"
}
