{self, ...}: {
  perSystem = {pkgs, ...}: {
    packages = let
      p = self.overlays.default null pkgs;
    in {
      inherit (p) albafetch;
      default = p.albafetch;
    };
  };
}
