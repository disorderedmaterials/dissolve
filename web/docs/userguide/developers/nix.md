---
title: Setting up a nix-based Development Environment
description: Instructions for creating a nix environment for Dissolve development.
---

## Introduction

The goal of this document is to enable you to setup a working
development environment, this tutorial is for developers on [IDAaaS](https://dev.analysis.stfc.ac.uk) 
as well as any Linux system to which you have sudo access, including WSL 2 on Windows.

## Setting up nix

[Nix](http://nixos.org) is a distribution independent package manager
which we will be using to install all of the libraries and tools needed
for building Dissolve.

### Install Nix

First, we need to run the nix installer. It will ask a series of
questions, to which the answer is always **yes**.

#### Linux

Install Nix with the [multi-user installation](https://nixos.org/manual/nix/stable/installation/multi-user.html):

```shell
sh <(curl -L https://nixos.org/nix/install) --daemon
```

### WSL 2

Install Nix with the [single-user installation](https://nixos.org/manual/nix/stable/installation/single-user.html).
Run this under your user account and not as `root`.

```shell
curl -L https://nixos.org/nix/install | sh -s -- --no-daemon
```

### Enable Flake support

With the release of nix version 3.0, flakes will be the preferred method
of handling nix packaging. However, version 2.0 isn\'t out yet, so we
need to tell nix that we will need flake support

First, we need to create the directory for the nix configuration file

```shell
mkdir -p ~/.config/nix
```

Now we need to add the `nix-command` and `flakes` features to nix

```shell
echo "experimental-features = nix-command flakes" >> ~/.config/nix/nix.conf
```

### Trust ourselves

So as not to waste hours recompiling code, we\'ll be downloading some of
our dependencies from a cache. We will need to add our cache to the
default one.

#### Linux

```shell
echo "substituters = https://cache.nixos.org https://dissolve-nix.cachix.org" | sudo tee -a /etc/nix/nix.conf
echo "trusted-public-keys = cache.nixos.org-1:6NCHdD59X431o0gWypbMrAURkbJ16ZPMQFGspcDShjY= nix-community.cachix.org-1:mB9FSh9qf2dCimDSUo8Zy7bkq5CX+/rkCWyvRCYg3Fs= dissolve-nix.cachix.org-1:b7kp954+mmuowr7XZsAIBugv+xp7+n3aSF/5obidRDE=" | sudo tee -a /etc/nix/nix.conf
sudo pkill nix-daemon
```

#### WSL 2

```shell
echo "substituters = https://cache.nixos.org https://dissolve-nix.cachix.org" | sudo tee -a ~/.config/nix/nix.conf
echo "trusted-public-keys = cache.nixos.org-1:6NCHdD59X431o0gWypbMrAURkbJ16ZPMQFGspcDShjY= nix-community.cachix.org-1:mB9FSh9qf2dCimDSUo8Zy7bkq5CX+/rkCWyvRCYg3Fs= dissolve-nix.cachix.org-1:b7kp954+mmuowr7XZsAIBugv+xp7+n3aSF/5obidRDE=" | sudo tee -a ~/.config/nix/nix.conf
sudo pkill nix-daemon
```

## Setting up the environment

If you have not done so already, close your terminal and start a new
session. The [*nix installer*]({{< ref "idaaas#install-nix" >}}) does
not take full effect until you start a fresh session.

### Upgrade `git`

IDAaaS has an outdated git version which runs into some issues with nix
accessing the latest dissolve version.

```shell
nix-env -i git
```

### Create an ssh key

We'll need an ssh key to get the source from GitHub and push our
changes. If you have an existing key, you can put that in the standard
place. Otherwise, you'll want to generate a new key.

```shell
ssh-keygen -t ecdsa
cat ~/.ssh/id_ecdsa.pub
```

The `cat` command will make the terminal print the public key
for your GitHub key. You\'ll need to add this as a new key on the
[GitHub new keys page](https://github.com/settings/keys).

### Get Dissolve

We download the source to Dissolve

```shell
git clone git@github.com:disorderedmaterials/dissolve.git
```

### Load the Dissolve environment

We\'ll now load the dissolve development environment. This will give us
access to all of the tools we need to build Dissolve.

First, make sure that we're in the Dissolve source directory.

```shell
cd dissolve
```

Now we start a development shell.

```shell
nix develop
```

There is a chance that, while setting up the shell, you\'ll get a
message about building `qtbase`. If so, go grab a cup of tea.
This means that one of our custom libraries hasn't been properly pulled
from cache and it will need some time to compile. Thankfully, this will
only ever need to happen once.

### Download the Submodules

We now need to download the git submodules.

``` shell
git submodule update --init --recursive
```

### Build Dissolve

We can now configure Dissolve.

```shell
mkdir build && cd build
cmake --preset GUI-nix ..
cd ..
```

And build

```shell
cmake --build build
```

Finally, test that Dissolve correctly compiled by running it

```shell
./build/bin/dissolve-gui
```

We can also run the test framework. Below, we use `-j8` because the default IDAaaS machine has eight cores. IF you're using a
different machine, adjust the number to match the number of physical
cores on your machine.

```shell
cd build
ctest -j8
```

## Optional - Home Manager Setup

[Home-manager](https://github.com/nix-community/home-manager) is a nix
utility for configuring a user's home environment. It provides a
convenient, unified interface for managing the configuration of
applications.

We can install home-manager in the same way we installed git.

```shell
nix-env -i home-manager
```

We then need to activate the home-manager environment for dissolve.

```shell
home-manager .#dissolve
```

The basic configuration includes a couple of basic quality of life
improvements for Dissolve development.

[direnv](https://direnv.net/)

: A utility for attaching settings to individual folders. With direnv
installed, it will no longer be necessary to run \`nix develop\` to
set all the shell variables. Instead, simply entering the
\`dissolve\` directory will cause the settings to load and leaving
the directoy will unload them.

[VS Code](https://code.visualstudio.com/)

: A popular, multi-language IDE.

[Ripgrep](https://github.com/BurntSushi/ripgrep)

: A fast and convenient utility for searching files. A simple \`rg
chart\` will display every instance of the word \`chart\` in all of
the source files in the project.

[Tealdeer](https://github.com/dbrgn/tealdeer)

: A quick reference for command line utilities. For example, \`tldr
rg\` will explain the basics of using ripgrep.

[Zoxide](https://github.com/ajeetdsouza/zoxide)

: A utility to quickly jump to other directories. If you have ever
visited the directory \`dissolve/src/data/ff/ludwig\`, then you can
immediately jump there with a simple \`z lud\`.

[noti](https://github.com/variadico/noti)
: Get notifications when a process finished. Run \`noti cmake --build
build\` and go check your e-mail. When the compile has finished, a
notification will pop up on the screen and inform you.
