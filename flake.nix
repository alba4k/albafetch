{
  description = "Faster neofetch alternative, written in C";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = {
    nixpkgs,
    flake-utils,
    self,
  }: let
    inherit (flake-utils.lib) defaultSystems eachDefaultSystem;
    version = builtins.substring 0 8 self.lastModifiedDate;

    packageFn = pkgs: {
      albafetch = pkgs.callPackage ./nix {inherit version;};
    };

    nixpkgsFor = nixpkgs.lib.genAttrs defaultSystems (system:
      import nixpkgs {
        inherit system;
        overlays = [self.overlays.default];
      });
  in
    {
      overlays.default = final: _: packageFn final;
    }
    // eachDefaultSystem (system: let
      pkgs = nixpkgsFor.${system};
    in {
      devShells = let
        inherit (pkgs.stdenv) mkDerivation;
      in {
        default = mkDerivation {
          name = "albafetch-shell";
          nativeBuildInputs = with pkgs; [pkg-config];
          buildInputs = with pkgs; [curl.dev meson ninja pciutils pkg-config];
        };
      };

      packages = let
        packages = with pkgs; {
          inherit albafetch;
          albafetch-static = pkgsStatic.albafetch;
        };
      in
        packages // {default = packages.albafetch;};
    })
    // {
      arm = let
        linuxPkgs = nixpkgsFor."x86_64-linux".pkgsCross.aarch64-multiplatform.pkgsStatic;
        darwinPkgs = nixpkgsFor."x86_64-darwin".pkgsCross.aarch64-darwin;
      in {
        linux = {inherit (linuxPkgs) albafetch;};
        darwin = {inherit (darwinPkgs) albafetch;};
      };
    };
}
