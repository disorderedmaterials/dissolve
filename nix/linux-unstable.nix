import (fetchTarball
"https://github.com/NixOS/nixpkgs/archive/e0508c81809b12013bce95562d556b1e672e3541.tar.gz") {
  overlays = [ (import ./overlays.nix) ];
}
