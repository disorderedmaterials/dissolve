#!/bin/sh
echo "use flake" > .envrc
direnv allow
"/root/.nix-profile/bin/nix" "develop" "--extra-experimental-features" "nix-command" "--extra-experimental-features" "flakes" "/dissolve#"
