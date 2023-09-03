{
  imports = [
    ./cross.nix
    ./dev.nix
    ./overlay.nix
    ./packages.nix
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
}
