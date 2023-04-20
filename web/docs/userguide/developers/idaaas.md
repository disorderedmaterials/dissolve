---
title: Setting up your Dissolve development environment
description: Instructions for creating a nix environment for developing Dissolve on IDAaaS
---
## [1.]{.section-number-2} Introduction {#orgbed6cc7}

The goal of this document is to enable you to setup a working
development environment on IDAaaS. It isn\'t specific to IDAaaS and
should work on any Linux system to which you have sudo access.

## [2.]{.section-number-2} Setting up nix {#org07d7e53}

[Nix](http://nixos.org) is a distribution independent package manager
which we will be using to install all of the libraries and tools needed
for building Dissolve.

### [2.1.]{.section-number-3} Install Nix {#orgc89c6bc}

First, we need to run the nix installer. It will ask a series of
questions, to which the answer is always **yes**.

``` {.src .src-shell}
sh <(curl -L https://nixos.org/nix/install) --daemon
```

### [2.2.]{.section-number-3} Enable Flake support {#org031a355}

With the release of nix version 3.0, flakes will be the preferred method
of handling nix packaging. However, version 2.0 isn\'t out yet, so we
need to tell nix that we will need flake support

First, we need to create the directory for the nix configuration file

``` {.src .src-shell}
mkdir -p ~/.config/nix
```

Now we need to add the `nix-command` and `flakes` features to nix

``` {.src .src-shell}
echo "experimental-features = nix-command flakes" >> ~/.config/nix/nix.conf
```

### [2.3.]{.section-number-3} Trust ourselves {#org44b9ce4}

So as not to waste hours recompiling code, we\'ll be downloading some of
our dependencies from a cache. We will need to add our cache to the
default one.

``` {.src .src-shell}
echo "substituters = https://cache.nixos.org https://dissolve-nix.cachix.org" | sudo tee -a /etc/nix/nix.conf
echo "trusted-public-keys = cache.nixos.org-1:6NCHdD59X431o0gWypbMrAURkbJ16ZPMQFGspcDShjY= nix-community.cachix.org-1:mB9FSh9qf2dCimDSUo8Zy7bkq5CX+/rkCWyvRCYg3Fs=" | sudo tee -a /etc/nix/nix.conf
sudo pkill nix-daemon
```

## [3.]{.section-number-2} Setting up the environment {#orgca5af1a}

If you have not done so already, close your terminal and start a new
session. The [nix installer](#orgc89c6bc) does not take full effect
until you start a fresh session.

### [3.1.]{.section-number-3} Upgrade `git` {#orga253485}

IDAaaS has an outdated git version which runs into some issues with nix
accessing the latest dissolve version.

``` {.src .src-shell}
nix-env -i git
```

### [3.2.]{.section-number-3} Create an ssh key {#orgdad2bc5}

We\'ll need an ssh key to get the source from GitHub and push our
changes. If you have an existing key, you can put that in the standard
place. Otherwise, you\'ll want to generate a new key.

``` {.src .src-shell}
ssh-keygen -t ecdsa
cat ~/.ssh/id_ecdsa.pub
```

The `cat` command will make the terminal print the public key for your
GitHub key. You\'ll need to add this as a new key on the [GitHub new
keys page](https://github.com/settings/keys).

### [3.3.]{.section-number-3} Get Dissolve {#orgaa3fe9e}

We download the source to Dissolve

``` {.src .src-shell}
git clone git@github.com:disorderedmaterials/dissolve.git
```

### [3.4.]{.section-number-3} Load the Dissolve environment {#orgbcaaf3d}

We\'ll now load the dissolve development environment. This will give us
access to all of the tools we need to build Dissolve.

First, make sure that we\'re in the Dissolve source directory.

``` {.src .src-shell}
cd dissolve
```

Now we start a development shell.

``` {.src .src-shell}
nix develop
```

There is a chance that, while setting up the shell, you\'ll get a
message about building `qtbase`. If so, go grab a cup of tea. This means
that one of our custom libraries hasn\'t been properly pulled from cache
and it will need some time to compile. Thankfully, this will only ever
need to happen once.

### [3.5.]{.section-number-3} Build Dissolve {#org68f8324}

We can now configure Dissolve.

``` {.src .src-shell}
mkdir build
cmake -G Ninja -DBUILD_SYSTEM_TESTS=ON -DBUILD_UNIT_TESTS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DGUI=ON -DMULTI_THREADING=ON -DBUILD_BENCHMARKS=ON  -DAntlrRuntime_INCLUDE_DIRS=$AntlrRuntime_INCLUDE_DIRS -DAntlrRuntime_LINK_DIRS=$AntlrRuntime_LINK_DIRS ..
cd ..
```

And build

``` {.src .src-shell}
cmake --build build
```

Finally, test that Dissolve correctly compiled by running it

``` {.src .src-shell}
./build/bin/dissolve-gui
```

We can also run the test framework. Below, we us `-j8` because the
default IDAaaS machine has eight cores. IF you\'re using a different
machine, adjust the number to match the number of physical cores on your
machine.

``` {.src .src-shell}
cd build
ctest -j8
```

## [4.]{.section-number-2} Remaining Tasks `[0/4]` {#org68d610b}


### [4.1.]{.section-number-3} [TODO]{.todo .TODO} CMake Presets {#orgc31ea24}

The command in [3.5](#org68f8324) is unwieldy. We need to create a
simple cmake presset. To handle this invocation.

### [4.2.]{.section-number-3} [TODO]{.todo .TODO} `ccache` support {#org18e62a3}

The dev environment supports [ccache](https://ccache.dev/). We should
add instructions for establishing a ccache and saving development time
on recompiles.

### [4.3.]{.section-number-3} [TODO]{.todo .TODO} direnv {#orgebb818c}

If we install [direnv](https://direnv.net/) into the global environment,
then the user won\'t need to manually trigger `nix develop` whenever
they start working, as it will be automatically loaded when they enter
the directory. More importantly, IDEs (e.g.VS Code, Emacs, CLion) can
link into direnv to find all the tools.

### [4.4.]{.section-number-3} [TODO]{.todo .TODO} Editors {#orgcb11d67}

By default, IDAaaS supplies [Sublime
Text](https://www.sublimetext.com/). There is a [direnv
plugin](https://packagecontrol.io/packages/Direnv) for Sublime, so it
would be possible to use it as an editor, but it\'s

We should update the development environment to include VS Code, as most

Author: Adam Washington

Created: 2023-04-13 Thu 09:36
