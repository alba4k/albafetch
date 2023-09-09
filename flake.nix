{
  description = "Faster neofetch alternative, written in C";

  nixConfig = {
    extra-substituters = ["https://albafetch.cachix.org"];
    extra-trusted-public-keys = ["albafetch.cachix.org-1:wTFl2hoUXiYlzUp/XA/bfZTP5KqZTToyq0+sfIVseUU="];
  };

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";

    parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };

    flake-compat = {
      url = "github:edolstra/flake-compat";
      flake = false;
    };
  };

  outputs = {parts, ...} @ inputs:
    parts.lib.mkFlake {inherit inputs;} {
      imports = [
        ./nix/dev.nix
        ./nix/overlay.nix
        ./nix/packages.nix
      ];

      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      perSystem = {pkgs, ...}: {
        formatter = pkgs.alejandra;
      };
    };
}
