{
  lib,
  curl,
  darwin,
  meson,
  ninja,
  pciutils,
  pkgconfig,
  stdenv,
  version,
}: let
  inherit (lib) cleanSource licenses maintainers optionals platforms;
  inherit (stdenv) isLinux isDarwin;
in
  stdenv.mkDerivation rec {
    name = "albafetch";
    inherit version;
    src = cleanSource ../.;

    buildInputs =
      [
        curl.dev
      ]
      ++ optionals isLinux [
        pciutils
      ];

    nativeBuildInputs =
      [meson ninja pkgconfig]
      ++ optionals isDarwin (with darwin.apple_sdk.frameworks; [
        Foundation
        IOKit
      ]);

    OBJC =
      if isDarwin
      then "clang"
      else "";

    meta = {
      description = "Faster neofetch alternative, written in C.";
      homepage = "https://github.com/alba4k/albafetch";
      license = licenses.mit;
      maintainers = with maintainers; [getchoo];
      platforms = platforms.unix;
    };
  }
