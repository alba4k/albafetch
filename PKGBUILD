# Maintainer: alba4k <blaskoazzolaaaron@gmail.com>
pkgname=albafetch-git
pkgver=2.5.r304.764303b
pkgrel=1
pkgdesc="Noefetch, but written in C; both faster and worse than the original"
arch=(x86_64)
url="https://github.com/alba4k/albafetch"
license=('MIT')
makedepends=(git make gcc)
source=("git+$url")
md5sums=('SKIP')

pkgver() {
    cd "${_pkgname}"
    printf "2.5.r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

package() {
    cd albafetch
    make DESTDIR="$pkgdir" install
    install -Dm644 LICENSE "/usr/share/licenses/${pkgname}/LICENSE"
    install -Dm644 README.md "/usr/share/doc/${pkgname}/README.md"
}
