#!/bin/sh
echo "use flake" > .envrc
direnv allow
"/root/.ni x-profile/bin/nix" "develop" "--extra-experimental-features" "nix-command" "--extra-experimental-features" "flakes" "/dissolve#"
