#!/usr/bin/env bash
nix-build -o dissolve.docker docker.nix
nix-build -o dissolve-oci.tar.bz2 oci.nix
nix-build -o dissolve.snap snap.nix
