{
  lib,
  clangStdenv,
  curl,
  pciutils,
  version,
}: let
  inherit (lib) licenses maintainers platforms;
in
  clangStdenv.mkDerivation rec {
    name = "albafetch";
    inherit version;
    src = lib.cleanSource ../.;

    stdenv = clangStdenv;
    makeFlags = ["CC=clang"];

    nativeBuildInputs = [curl pciutils];

    installPhase = ''
      outdir=$out/bin
      mkdir -p "$outdir"
      install -Dm755 build/${name} "$outdir"/${name}
    '';

    meta = {
      description = "Faster neofetch alternative, written in C.";
      homepage = "https://github.com/alba4k/albafetch";
      license = licenses.mit;
      maintainers = with maintainers; [getchoo];
      platforms = with platforms; lib.flatten [linux darwin];
    };
  }
