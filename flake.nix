{
  description = "Faster neofetch alternative, written in C";

  nixConfig = {
    extra-substituters = [ "https://albafetch.cachix.org" ];
    extra-trusted-public-keys = [
      "albafetch.cachix.org-1:wTFl2hoUXiYlzUp/XA/bfZTP5KqZTToyq0+sfIVseUU="
    ];
  };

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:

    flake-utils.lib.eachDefaultSystem (
      system:

      let
        inherit (nixpkgs) lib;
        pkgs = nixpkgs.legacyPackages.${system};

        callWith = pkgs: (lib.makeScope pkgs.newScope (lib.flip self.overlays.default pkgs)).albafetch;

        armPkgs = pkgs.pkgsCross.aarch64-multiplatform;
        albafetch-static = callWith pkgs.pkgsStatic;
      in

      {
        devShells.default = pkgs.mkShell {
          inputsFrom = [ self.packages.${system}.albafetch ];
        };

        formatter = pkgs.alejandra;

        packages = {
          albafetch = callWith pkgs;
          default = self.packages.${system}.albafetch;
        };

        legacyPackages =
          {
            x86_64-linux = {
              albafetch-arm = callWith armPkgs.pkgsStatic;
              inherit albafetch-static;
            };

            aarch64-linux = { inherit albafetch-static; };

            x86_64-darwin = {
              albafetch-arm = callWith armPkgs;
            };
          }
          .${system} or { };
      }
    )
    // {
      overlays.default = final: _: {
        albafetch = final.callPackage ./nix/package.nix { inherit self; };
      };
    };
}
