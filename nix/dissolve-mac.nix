{ parallel ? false, gui ? false }:

import ./dissolve.nix {pkgs=(import ./mac.nix); unstable=(import ./mac.nix);}
