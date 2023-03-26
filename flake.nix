{
  description = "Faster neofetch alternative, written in C";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = {
    nixpkgs,
    flake-utils,
    self,
  }: let
    inherit (flake-utils.lib) eachDefaultSystem;
    version = builtins.substring 0 8 self.lastModifiedDate;

    packageFn = pkgs: {
      albafetch = pkgs.callPackage ./nix {inherit version;};
    };
  in
    {
      overlays.default = final: _: packageFn final;
    }
    // eachDefaultSystem (system: let
      pkgs = import nixpkgs {
        inherit system;
        overlays = [self.overlays.default];
      };
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
        pkgs = import nixpkgs {
          system = "x86_64-linux";
          overlays = [self.overlays.default];
        };
      in {
        inherit (pkgs.pkgsCross.aarch64-multiplatform.pkgsStatic) albafetch;
      };
    };
}
