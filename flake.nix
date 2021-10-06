{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.05";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem ({
      defaultPackage = nixpkgs.mkDerivation {
        pname = "dissolve";
        version = "0.9.0";
        src = ./.;
        meta = with nixpkgs.lib; {
          description = "";
          homepage = "";
          # license = licenses.unlicense;
          maintainers = [ maintainers.rprospero ];
        };
      };
    });
}
