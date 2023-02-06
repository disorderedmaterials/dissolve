---
title: Singularity Containers
weight: 3
description: Using our Singularity images
---

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
