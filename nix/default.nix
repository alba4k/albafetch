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

    patches = [./meson.patch];
    buildInputs =
      [
        pciutils
      ]
      ++ optionals isLinux [
        curl.dev
      ]
      ++ optionals isDarwin (with darwin.apple_sdk.frameworks; [
        Foundation
        IOKit
      ]);

    nativeBuildInputs = [meson ninja pkgconfig];

    meta = {
      description = "Faster neofetch alternative, written in C.";
      homepage = "https://github.com/alba4k/albafetch";
      license = licenses.mit;
      maintainers = with maintainers; [getchoo];
      platforms = with platforms; lib.flatten [linux darwin];
    };
  }
