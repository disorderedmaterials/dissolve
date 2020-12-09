Version 0.7 makes significant changes to some of Dissolve’s workflows (in particular the interactions between the correlation modules which has been simplified), adds calculation of x-ray weighted structure factors and the capability to include those data within an EPSR refinement, improves the layout and usability of module controls and data within the GUI, and contains a myriad other bugfixes and improvements at the algorithm level.

### Correlation Modules

Previously, simulations typically used an `RDF` module followed by many `NeutronSQ` modules of which the first in the layer took the `RDF` data and Fourier-transformed it into the S(Q)/F(Q) for use by all subsequent `NeutronSQ` modules in the layer. This approach, although efficient, had significant drawbacks, and was not transparent to the user. In addition, different configurations than those specified in the `RDF` module could be targeted in the `NeutronSQ` modules, leading to further confusion, and potential nonsense results.

As of version 0.7, an `SQ` module must be used to perform the Fourier transform of `RDF` data. The `NeutronSQ` (and now `XRaySQ`) modules then simply weight this data. Extraneous keyword options have been removed from `NeutronSQ`, and neither it nor `XRaySQ` target any configurations. Instead, they reference the `SQ` module containing the structure factors to apply weighting to. In a similar spirit, the `SQ` module refers only to the source `RDF` module. As such, existing input files will need to be modified in order to include the necessary `SQ` module in relevant workflows.

### Restart File Compatibility

Restart files from previous version are highly likely to be incompatible with version 0.7, since changes to the naming and formatting of core class I/O has changed.

### Summary of Major Changes
- Added XRaySQ module.
- Extend EPSRModule to handle x-ray datasets.
- Allow averaging of structure factors.
- Enforce more consistent relationships between correlation-type modules (i.e. `RDF`, `SQ`, `NeutronSQ`, and `XRaySQ`).
- Display module controls and output within the layer, rather than allowing it to be open in a separate tab.
- New forcefields: PCL2019 (ionic liquids), Kulmala2010 (aqueous H2SO4).
- Conversion of many base classes to use STL algorithms and template classes.
