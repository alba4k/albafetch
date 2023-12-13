{self, ...}: {
  perSystem = {
    pkgs,
    system,
    config,
    ...
  }: let
    packageFn = pkgs: (self.overlays.default pkgs pkgs).albafetch;

    armPkgs = pkgs.pkgsCross.aarch64-multiplatform;
    albafetch-static = packageFn pkgs.pkgsStatic;
  in {
    packages = {
      albafetch = packageFn pkgs;
      default = config.packages.albafetch;
    };

    legacyPackages =
      {
        "x86_64-linux" = {
          albafetch-arm = packageFn armPkgs.pkgsStatic;
          inherit albafetch-static;
        };

        "aarch64-linux" = {inherit albafetch-static;};

        "x86_64-darwin" = {
          albafetch-arm = packageFn armPkgs;
        };
      }
      .${system}
      or {};
  };
}
