import (fetchTarball
"https://github.com/NixOS/nixpkgs/archive/fd341a576e1b82e4d6db7f9a36b9373a870bffce.tar.gz") {
  overlays = [ (import ./overlays.nix) ];
}
