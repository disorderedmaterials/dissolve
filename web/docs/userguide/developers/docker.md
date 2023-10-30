---
title: Setting up a dev environment using Docker for Windows
description: Instructions for building and running a dissolve Docker image on Windows, including GUI support
---

## Introduction

Outlined below is a step-by-step guide to setting up a development environment on Windows using [Docker](https://www.docker.com/products/docker-desktop/) containerisation.

While a Dockerfile for building the package can be found at the top level of the dissolve repository, some manual steps are involved, and this
page aims to guide you through them.

### Basic requirments

You'll need to be running Windows with Docker desktop and the WSL ([Windows Subsystem for Linux](https://learn.microsoft.com/en-us/windows/wsl/install)) installed.
Check out the dissolve source repository to get access to the code and Dockerfile.

### GUI support with X11 forwarding

For use of the dissolve GUI software, we will need to have installed an X11-forwarding utility on the Windows host system.
This workflow uses the [Xming X Server](http://www.straightrunning.com/XmingNotes/).

## Setting up the environment

### 1) Build the Docker image

In the dissolve code, open a command prompt of your choice (for instance cmd.exe, powershell, or git bash) and run
```shell
Docker build -t dissolve .
```
in order to build the image, where "dissolve" is the image tag.

The image layers are typically cached by Docker so the more time consuming parts of the build process (ranging to a couple of hours) only have to be done on the first build. Subsequent builds are quick.

### 2) Create an X11 client on the host

Open XLaunch, the X11 wizard installed with Xming.

You will be prompted to choose some display settings for viewing the dissolve GUI. It is recommend that the first option (Multiple windows)
is used, since this tends to allow the detached GUI to integrate neatly with your other windows.

Click Next>, allowing the program to start without a client, since this wil automatically detect the container once we run it.

Clicking Next> again, we must allow "Disable access control".

Then configuration is complete. If the XLaunch wizard is successfully running, an XLaunch logo should be visible in the bottom right of the Windows homescreen along with other running applications.

### 3) Run the container and build the dissolve dev environment

Once again in the command prompt, run
```shell
Docker run -it -u 0 -e DISPLAY=$HOST_IP_ADDRESS:0 -v $PATH_TO_DISSOLVE_CODE_ON_HOST:/dissolve --name $CONTAINER_NAME dissolve
```

This will run a new instance of the built image in an interactive terminal - you should now see the container's development shell running.

Notice that this command maps the IP address of the host machine to the container (a requirement for X11 forwarding of graphics to take place) as well as mapping the dissolve code into the container. 

To build the dissolve software using `cmake`, you will need to run the following commands
```shell
nix-env -iA nixpkgs dos2unix; dos2unix flake.nix    # removes any problematic DOS characters inside the flake file
cmake --preset GUI-nix
cmake --build build
```
The build products should now be visible in the `/build` directory on both the host and container file systems.

To run all tests from the top level of the repo, run `cd build; ctest -j4`.

### 4) Launch the dissolve GUI

Moving over to the interactive terminal of the container, we should see that we are in the container `/dissolve` directory.
Run `nixGLIntel ./build/bin/dissolve-gui` to start the software.

You should now see the dissolve home screen open in a new window.