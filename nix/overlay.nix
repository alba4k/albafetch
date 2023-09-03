{self, ...}: {
  flake.overlays.default = _: prev: let
    version = builtins.substring 0 8 self.lastModifiedDate or "dirty";
  in {
    albafetch = prev.callPackage ./derivation.nix {
      inherit (prev.darwin.apple_sdk.frameworks) Foundation IOKit;
      inherit self version;
    };
  };
}
