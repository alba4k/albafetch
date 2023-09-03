{
  self,
  withSystem,
  ...
}: {
  flake.crossPackages = let
    getPkgs = system: withSystem system (p: p.pkgs);
    linuxPkgs = getPkgs "x86_64-linux";
    darwinPkgs = getPkgs "x86_64-darwin";

    genPkg = pkgs: {inherit (self.overlays.default null pkgs) albafetch;};
  in {
    arm = {
      linux = genPkg linuxPkgs.pkgsCross.aarch64-multiplatform.pkgsStatic;
      darwin = genPkg darwinPkgs.pkgsCross.aarch64-darwin;
    };

    static = {
      linux = genPkg linuxPkgs.pkgsStatic;
      darwin = genPkg darwinPkgs.pkgsStatic;
    };
  };
}
