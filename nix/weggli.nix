{pkgs}:

pkgs.rustPlatform.buildRustPackage rec {
  pname = "weggli";
  version = "12.1.1";

  src = pkgs.fetchFromGitHub {
    owner = "googleprojectzero";
    repo = pname;
    rev = "main";
    sha256 = "EjfZh9wJmLQUvqrHNwj7qAZdGNQZs1aeowkyEgRztU0=";
  };

  cargoSha256 = "/aI7Gtv0NzDLCAtniGOKzEX2vAHnB+VM0ejkPoVyLpc=";
  default = false;
  wayland = true;

  meta = with pkgs.lib; {
    description = " fast and robust semantic search tool for C and C++ codebases";
    homepage = "https://github.com/googleprojectzero/weggli";
    maintainers = [ maintainers.rprospero ];
  };
}
