# nix/package.nix
{
  lib,
  stdenv,
  apple-sdk_14,
  meson,
  ninja,
  vulkan-headers,
  vulkan-loader,
  pkg-config,
  sqlite,

  self ? { },
}:

let
  latestTag = "4.3";

  fullDate = lib.substring 0 8 self.lastModifiedDate;

  formattedDate =
    if (self ? "lastModifiedDate") then
      lib.concatStringsSep "-" [
        (lib.substring 0 4 fullDate)
        (lib.substring 4 2 fullDate)
        (lib.substring 6 2 fullDate)
      ]
    else
      "unknown";
in

stdenv.mkDerivation {
  name = "albafetch";
  version = "${latestTag}-unstable-${formattedDate}";

  src = self;

  buildInputs =
    [
      sqlite
      vulkan-headers
      vulkan-loader
    ]
    ++ lib.optional stdenv.hostPlatform.isDarwin apple-sdk_14;

  nativeBuildInputs = [
    meson
    ninja
    pkg-config
  ];

  OBJC = lib.optionalString stdenv.hostPlatform.isDarwin "clang";

  postFixup = lib.optionalString stdenv.hostPlatform.isStatic ''
    rm -r $out/nix-support
  '';

  meta = {
    description = "Faster neofetch alternative, written in C.";
    homepage = "https://github.com/alba4k/albafetch";
    license = lib.licenses.mit;
    maintainers = with lib.maintainers; [ getchoo ];
    platforms = lib.platforms.unix;
  };
}
