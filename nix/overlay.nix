{ self, ... }:
{
  flake.overlays.default = final: prev: {
    albafetch = prev.callPackage ./derivation.nix {
      inherit self;
    };
  };
}
