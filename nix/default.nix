{
  lib,
  curl,
  meson,
  ninja,
  pciutils,
  pkgconfig,
  stdenv,
  version,
}: let
  inherit (lib) cleanSource licenses maintainers platforms;
in
  stdenv.mkDerivation rec {
    name = "albafetch";
    inherit version;
    src = cleanSource ../.;

    patches = [./meson.patch];
    buildInputs = [curl.dev pciutils];
    nativeBuildInputs = [meson ninja pkgconfig];

    meta = {
      description = "Faster neofetch alternative, written in C.";
      homepage = "https://github.com/alba4k/albafetch";
      license = licenses.mit;
      maintainers = with maintainers; [getchoo];
      platforms = with platforms; lib.flatten [linux darwin];
    };
  }
