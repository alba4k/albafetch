{
  lib,
  stdenv,
  meson,
  ninja,
  pciutils,
  pkg-config,
  Foundation,
  IOKit,
  self,
}:
stdenv.mkDerivation {
  name = "albafetch";
  version = builtins.substring 0 8 self.lastModifiedDate or "dirty";

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
      pkg-config
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
