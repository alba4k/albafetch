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
}: let
  filter = path: type: let
    path' = toString path;
    base = baseNameOf path';

    extensions = [".c" ".h" ".m"];
    matches = lib.any (suffix: lib.hasSuffix suffix base) extensions;

    isAllowedDir = type == "directory" && base == "src";
    isMesonFile = base == "meson.build";
  in
    matches || isAllowedDir || isMesonFile;

  filterSource = src:
    lib.cleanSourceWith {
      src = lib.cleanSource src;
      inherit filter;
    };
in
  stdenv.mkDerivation {
    name = "albafetch";
    version = builtins.substring 0 8 self.lastModifiedDate or "dirty";

    src = filterSource self;

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
