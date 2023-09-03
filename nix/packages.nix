{self, ...}: {
  perSystem = {
    lib,
    pkgs,
    ...
  }: {
    packages = let
      p = lib.fix (final: self.overlays.default ({inherit (pkgs) darwin;} // final) pkgs);
    in {
      inherit (p) albafetch;
      default = p.albafetch;
    };
  };
}
