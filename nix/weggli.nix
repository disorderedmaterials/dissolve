{ pkgs, src }:

pkgs.rustPlatform.buildRustPackage rec {
  pname = "weggli";
  version = "0.2.2";

  inherit src;

  cargoSha256 = "9vQ58kw64ok714qtj0xkcaMUQlGwUHwxg6ycSE/gtOQ=";
  default = false;
  wayland = true;

  meta = with pkgs.lib; {
    description =
      " fast and robust semantic search tool for C and C++ codebases";
    homepage = "https://github.com/googleprojectzero/weggli";
    maintainers = [ maintainers.rprospero ];
  };
}
