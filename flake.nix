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
    eachDefaultSystem (system: let
      pkgs = import nixpkgs {inherit system;};
    in {
      devShells = let
        inherit (pkgs.clangStdenv) mkDerivation;
      in {
        default = mkDerivation {
          name = "albafetch-shell";
          buildInputs = with pkgs; [curl gnumake pciutils];
        };
      };

      packages = let
        packages = packageFn pkgs;
      in
        packages // {default = packages.albafetch;};
    })
    // {overlays.default = final: _: packageFn final;};
}
