{self, ...}: {
  perSystem = {
    lib,
    pkgs,
    system,
    self',
    ...
  }: {
    packages = let
      armPkgs = pkgs.pkgsCross.aarch64-multiplatform;

      fn = pkgs: (self.overlays.default pkgs pkgs).albafetch;
    in
      {
        albafetch = fn pkgs;
        default = self'.packages.albafetch;
      }
      // lib.optionalAttrs (system == "x86_64-linux") {
        albafetch-arm = fn armPkgs.pkgsStatic;
      }
      // lib.optionalAttrs (system == "x86_64-darwin") {
        albafetch-arm = fn armPkgs;
      }
      // lib.optionalAttrs pkgs.stdenv.hostPlatform.isLinux {
        albafetch-static = fn pkgs.pkgsStatic;
      };
  };
}
