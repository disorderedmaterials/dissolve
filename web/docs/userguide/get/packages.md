---
title: Pre-Built Packages
description: Pre-built packages for different operating systems
weight: 2
---

Dissolve is available in packaged / installable form for several common operating systems from the [packages](https://www.projectdissolve.com/packages) page.  Note that these packages focus primarily on providing the GUI version of the code, which can be used for setting up / visualising simulations, as well as running smaller-scale simulations such as the [examples]({{< ref "examples" >}}).

### Windows 10

Dissolve comes in both Windows installer and zip forms. The latter is provided for those of us who don't have administrator rights on our work machines (because we can't be trusted) and so need a flat directory of the package that can be put anywhere.

Being a C++ code, Dissolve also needs the Microsoft Visual C++ Runtime Library to be installed, otherwise you'll get errors like this:

{{< cimage src="../windows-msvcrt-error.png" >}}

You can download the official installer from Microsoft at https://aka.ms/vs/17/release/vc_redist.x64.exe.  I agree that this also looks like a fake or nefarious link, so if in any doubt go through the parent page at https://learn.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist instead. Please don't download individual dll files from the internet in order to fix the problem - those files have no provenance, and there is no guarantee that they are what they say they are.

### Mac OSX

Starting from version 1.5.0 [(continuous release)](https://github.com/disorderedmaterials/dissolve/releases/tag/continuous) we provide a Dissolve DMG compatible with Apple Silicon (ARM64).

On MacOS, after installing the application you must run these following commands:

```
cd /Applications
xattr -rd com.apple.quarantine Dissolve-GUI-<Processor>.app
xattr -rd com.apple.quarantine  Dissolve-GUI-<Processor>.app/Contents/MacOS/dissolve-gui
chmod +x  Dissolve-GUI-<Processor>.app/Contents/MacOS/dissolve-gui
```

You will also need to have [FTGL](https://formulae.brew.sh/formula/ftgleforge.net/projects/ftgl/) installed on your system, which is used for rendering fonts. The libraries should be installed using homebrew, as pre-built Dissolve is linked to the homebrew directories.

### Linux


We provide Singularity images of Dissolve in an attempt to cover the needs of many / most / some Linux users. The alternative is to [compile it yourself]({{< ref "compilation" >}}).

You will need to have the main [`singularity` package installed](https://docs.sylabs.io/guides/3.0/user-guide/installation.html) on your system. Once done, running the container should be as simple as:

```
bob@linux:~> singularity run dissolve-0.9.1.sif
```

When running the GUI image you may well find that this doesn't quite work, with errors along the lines of the following:

```
WARNING: passwd file doesn't exist in container, not updating
WARNING: group file doesn't exist in container, not updating
WARNING: Skipping mount /etc/localtime [binds]: /etc/localtime doesn't exist in container
No protocol specified
qt.qpa.xcb: could not connect to display :1
qt.qpa.plugin: Could not load the Qt platform plugin "xcb" in "" even though it was found.
This application failed to start because no Qt platform plugin could be initialized. Reinstalling the application may fix this problem.

Available platform plugins are: eglfs, linuxfb, minimal, minimalegl, offscreen, vkkhrdisplay, vnc, wayland-egl, wayland, wayland-xcomposite-egl, wayland-xcomposite-glx, xcb.

/nix/store/3cwnv48rpwzsx62nmgpfy7xn5p4hhxpa-nixGLIntel/bin/nixGLIntel: line 5: 77941 Aborted                 (core dumped) "$@"
```

The important error here is `qt.qpa.xcb: could not connect to display :1` - you'll need to allow the singularity image to connect to your local display:

```
bob@linux:~> xhost + local:
```

This allows local, non-network connections to your X display (note the trailing `:` on `local`).

Finally, you will probably need to let the container have read-only access to a system directory so it knows what it's doing. We can do this with the `-B` option to the run command:

```
bob@linux:~> singularity run -B/etc:/etc:ro dissolve-gui-0.9.1.sif
```

and which should now let the Dissolve GUI run properly. If the GUI runs correctly but displays odd characters or looks like it is missing a font entirely, add `-B/usr/share:/usr/share:ro` to the singularity command. You may need to adjust this depending on where fonts are stored on your system.
