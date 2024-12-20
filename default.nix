{
  pkgs ? import <nixpkgs> {
    inherit system;
    config = { };
    overlays = [ ];
  },
  system ? builtins.currentSystem,
}:

{
  albafetch = pkgs.callPackage ./nix/package.nix { };
}
