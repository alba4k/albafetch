{
  lib,
  stdenv,
  curl,
  meson,
  ninja,
  pciutils,
  pkgconfig,
  Foundation,
  IOKit,
  self,
  version,
}:
stdenv.mkDerivation {
  name = "albafetch";
  inherit version;
  src = lib.cleanSource self;

  buildInputs =
    [
      curl.dev
    ]
    ++ lib.optional stdenv.isLinux pciutils;

  nativeBuildInputs =
    [
      meson
      ninja
      pkgconfig
    ]
    ++ lib.optionals stdenv.isDarwin [
      Foundation
      IOKit
    ];

  OBJC =
    if stdenv.isDarwin
    then "clang"
    else "";

  meta = with lib; {
    description = "Faster neofetch alternative, written in C.";
    homepage = "https://github.com/alba4k/albafetch";
    license = licenses.mit;
    maintainers = with maintainers; [getchoo];
    platforms = platforms.unix;
  };
}
