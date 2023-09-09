{self, ...}: {
  flake.overlays.default = final: prev: {
    albafetch = prev.callPackage ./derivation.nix {
      inherit (final.darwin.apple_sdk.frameworks) Foundation IOKit;
      inherit self;
    };
  };
}
