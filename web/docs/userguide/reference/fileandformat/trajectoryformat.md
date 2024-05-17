---
title: Trajectory
description: Trajectory import/export
---

## Import

### File Types

|Keyword|Description|
|:---:|-----------|
|`xyz`|Appended XMol-style xyz coordinates. Line 1 contains the number of atoms N. Line 2 contains a title string. The next N lines contain "element  rx  ry  rz". This format is repeated for each frame.|

### Options

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|

## Export

### File Types

|Keyword|Description|
|:---:|-----------|
|`xyz`|Appended XMol-style xyz coordinates. Line 1 contains the number of atoms N. Line 2 contains a title string. The next N lines contain "element  rx  ry  rz". This format is repeated for each frame.|
|`xyzExt`|Extended XMol-style xyz coordinates. Mostly identical to `xyz`, but with two additional columns appended to the line for each atom.  These columns contain first the index of the atom within the molecule, followed by the atom type|
