Version 0.8 adds a significant amount of new functionality, bugfixes, and performance gains.

## Major Changes

### Parallelism

Version 0.8.0 moves to multithreading via Threading Building Blocks / Parallel STL as its principal means of parallelisation, departing from the use of pure MPI in version 0.7.X and earlier. The advantages of this approach are a significant reduction in memory usage as well as considerable speedups across most routines thanks to the now-widespread use of standard containers in the code. However, at present it is recommended to run Dissolve only on a single physical machine, in contrast to previous versions which could (in principle) scale across any number of nodes. The full testing and implementation of MPI with multithreading is [scheduled for version 0.9.0](https://projectdissolve.com/docs/roadmap/).

One significant benefit of this approach is that the default builds of the GUI are now also multithreaded, providing significant performance gains in desktop environments. 

### Bragg Scattering

The new [Bragg module](https://projectdissolve.com/docs/modules/correlation/bragg/) calculates Bragg intensities over a specified Q-range, and which can then be taken by the SQ module, broadened, and incorporated into the total structure factors. Related enhancements to this module (e.g. more broadening functional forms, better blending of Bragg/diffuse regions), will likely be incorporated as patch versions.

### Restart File Compatibility

Restart files from previous versions are not compatible with version 0.8 since significant changes to the way some data are formatted have been made in order to reduce the size of the restart files.

## Other Significant Changes
- Trajectories with variable cell size (i.e. NPT simulations) can now be read correctly.
- New Accumulation module to allow averaging of calculated partial sets.
- Allow use of atom geometry specification in NETA descriptions.
- Benchmarking of various functions / processes is now performed in our CI via GitHub Actions ([results can be found here](https://disorderedmaterials.github.io/dissolve/dev/bench/)).
- Significant removal of custom class use, moving to standard containers.
- More control over location of species addition to configurations via regions.
- Extend unit testing to some GUI functionality, and begin moving to model-view approach.
