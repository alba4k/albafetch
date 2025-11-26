{
  lib,
  stdenv,
  apple-sdk_14,
  meson,
  ninja,
  pciutils,
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
        # YYYY
        (lib.substring 0 4 fullDate)
        ## MM
        (lib.substring 4 2 fullDate)
        # DD
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
    ]
    ++ lib.optional stdenv.hostPlatform.isDarwin apple-sdk_14
    ++ lib.optional stdenv.hostPlatform.isLinux pciutils;

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
