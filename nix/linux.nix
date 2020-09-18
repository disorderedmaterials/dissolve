import (fetchTarball
"https://github.com/NixOS/nixpkgs/archive/252bfe0107587d40092057f338e9ffcf7bbd90cb.tar.gz") {
  overlays = [ (import ./overlays.nix) ];
}
