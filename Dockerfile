# Nix builder
FROM nixos/nix:latest AS builder

RUN echo "substituters = https://cache.nixos.org https://dissolve-nix.cachix.org" > /etc/nix/nix.conf
RUN echo "trusted-public-keys = cache.nixos.org-1:6NCHdD59X431o0gWypbMrAURkbJ16ZPMQFGspcDShjY= nix-community.cachix.org-1:mB9FSh9qf2dCimDSUo8Zy7bkq5CX+/rkCWyvRCYg3Fs= dissolve-nix.cachix.org-1:b7kp954+mmuowr7XZsAIBugv+xp7+n3aSF/5obidRDE=" >> /etc/nix/nix.conf
RUN echo "experimental-features = nix-command flakes" >> /etc/nix/nix.conf
RUN /root/.nix-profile/bin/nix build github:disorderedmaterials/dissolve#devShells.x86_64-linux.default
RUN nix-env -iA nixpkgs.dos2unix
VOLUME /dissolve
WORKDIR /dissolve

CMD [ "/bin/sh", "-c", "./scripts/setup.sh" ]