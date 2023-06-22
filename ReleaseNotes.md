Version 1.2.0 adds new functionality

## Major Changes in v1.2

### External Potentials

Basic options to add additional potentials affecting all / selected atoms now exist, with the most useful being the introduction of harmonic point restraints for individual atoms (`SimpleRestraintPotential`). Future revisions to the code will introduce other interaction types.

### XRay Data Handling

The handling of x-ray data has been significantly improved, with the resolution of several issues making treatment of this data type much more robust.

### Species Charge Manipulation

Several new options for manipulating both species and atom type charges have been introduced in order to automate somewhat the process of "neutralising" a simulation, or achieving a specific charge on a species. This includes the ability to reduce the significant figures present in assigned charges, to assign atom type charges to the species atoms, to generate averaged species atom charges and set new atom type charges, and several others.

### Flexible Region Generation

Within a configuration generator it is now possible to define a `CustomRegion` based on evaluation of a mathematical equation, allowing useful / exotic regions to be created into which molecules can be added, or removed.

## Minor Changes in v1.2

- UI: Added icons for all nodes.
- Fix: Navigating the node list of a procedure with the cursor keys did not update the associated keyword data.
- Fix: Occasional crash when multiple components were added to a configuration in the generator.
