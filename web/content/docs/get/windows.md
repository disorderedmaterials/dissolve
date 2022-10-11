---
title: Windows Usage Guide
description: Installing and Running Dissolve on Windows systems
---

Dissolve comes in both Windows installer and zip forms. The latter is provided for those of us who don't have administrator rights on our work machines (because we can't be trusted) and so need a flat directory of the package that can be put anywhere.

Being a C++ code, Dissolve also needs the Microsoft Visual C++ Runtime Library to be installed, otherwise you'll get errors like this:

{{< cimage src="../windows-msvcrt-error.png" >}}

You can download the official installer from Microsoft at https://aka.ms/vs/17/release/vc_redist.x64.exe.  I agree that this also looks like a fake or nefarious link, so if in any doubt go through the parent page at https://learn.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist instead. Please don't download individual dll files from the internet in order to fix the problem - those files have no provenance, and there is no guarantee that they are what they say they are.
