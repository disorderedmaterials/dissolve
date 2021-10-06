{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.05";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:

      let
        pkgs = nixpkgs.legacyPackages.${system};
        dissolve = pkgs.gcc9Stdenv.mkDerivation {
          pname = "dissolve";
          version = "0.9.0";
          src =
            builtins.filterSource (path: type: baseNameOf path != "flake.nix")
            ./.;
          patches = [ ./nix/patches/no-conan.patch ];
          buildInputs = with pkgs; [
            antlr4
            antlr4.runtime.cpp
            bison
            cmake
            cli11
            fmt
            fmt.dev
            freetype
            jre
            openmpi
            pkgconfig
            pugixml
          ];
          cmakeFlags = [ "-DMULTI_THREADING=OFF" ];
          installPhase = ''
            mkdir -p $out/bin
            ls nix
            mv ./$out/bin/dissolve $out/bin/
          '';

          meta = with pkgs.lib; {
            description = "";
            homepage = "";
            # license = licenses.unlicense;
            maintainers = [ maintainers.rprospero ];
          };
        };
      in { defaultPackage = dissolve; });
}
