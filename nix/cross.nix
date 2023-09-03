{self, ...}: {
  perSystem = {
    lib,
    pkgs,
    ...
  }: {
    legacyPackages = let
      inherit (lib) optionalAttrs;
      inherit (pkgs.stdenv) isLinux;
      inherit (pkgs.stdenv.hostPlatform) isx86_64;

      arm = pkgs.pkgsCross.aarch64-multiplatform;

      fn = pkgs: lib.fix (final: self.overlays.default ({inherit (pkgs) darwin;} // final) pkgs);
      genPkg = pkgs: {inherit (fn pkgs) albafetch;};
    in
      optionalAttrs isx86_64 {
        arm = genPkg (
          if isLinux
          then arm.pkgsStatic
          else arm
        );
      }
      // optionalAttrs isLinux {
        static = genPkg pkgs.pkgsStatic;
      };
  };
}
