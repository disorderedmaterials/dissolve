#!/bin/sh
dos2unix flake.nix
"/root/.nix-profile/bin/nix" "develop" "--extra-experimental-features" "nix-command" "--extra-experimental-features" "flakes" "/dissolve#"
