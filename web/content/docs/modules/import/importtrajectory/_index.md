---
title: ImportTrajectory (Module)
linkTitle: ImportTrajectory
description: Read coordinates from a trajectory file into a configuration
---

## Overview

The `ImportTrajectory` module reads in a frame from a trajectory file on disk, storing the coordinates in a target configuration. Called repeatedly within the main simulation loop, the module reads in successive frames from the trajectory file, and so can be used to process an existing trajectory file within Dissolve.

The module requires that the contents of the configuration have been described in terms of species and their populations in the standard way, and assumes that the total number of atoms in the configuration is equal to the number per frame in the trajectory. No other information other than atomic coordinates is read.

`ImportTrajectory` stores the file position of the last frame read in the restart file, and so permits analysis pipelines to be restarted if required.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Format`|[`TrajectoryFileAndFormat`]({{< ref "trajectoryformat" >}})|--|Format and filename of the target trajectory to read from disk.|
