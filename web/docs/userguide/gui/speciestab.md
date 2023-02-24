---
title: Species Tab
description: Species tabs
weight: 2
---

A Species tab exposes all aspects of a single species defined in the simulation. Species can be created from the _Species_ menu, and can be removed from the simulation by clicking the close button associated with the tab. Various options to manipulate the currently-visible species (tab) can also be found on the _Species_ menu.

### Structure

{{< cimage src="../speciestab_structure.png" caption="Species tab showing the geometry of the defined molecule" width=90% >}}

The structure tab shows the atom geometry and connectivity as a fairly standard stick or ball and stick model. While you can rotate and zoom the view, and even minimise the geometry according to the currently-defined forcefield terms for the species, the atoms and their connectivity cannot be directly edited or added to. In other words, once a species is defined its atomic contents and connectivity are fixed for all time.

### Forcefield

{{< cimage src="../speciestab_forcefield.png" caption="Species tab showing its currently assigned forcefield terms" width=90% >}}

#### Atoms

The _Forcefield_ tab shows an explicit table of atoms and their coordinates, as well as their assigned atom types and charges, all of which can be directly edited. Note that these charges are the _Species Atom_ charges referred to in the _Charge Source_ section of the [forcefield tab]({{< ref "forcefieldtab" >}}). For reference, adjacent to the table of atoms are the calculated total charges from atom types and species atoms. Non-zero values will be highlighted. 

#### Bonds, Angles, Torsions, and Impropers

The remainder of the _Forcefield_ tab shows all defined intramolecular terms for the current species. Note that this may only be the bond (connectivity) terms if a forcefield has not been applied. Since this information is either fixed in the species (bonds) or determined by Dissolve (angles, torsions, and impropers, the latter from the applied forcefield) terms may not be added or deleted, but the functional form and parameters may be freely changed. Note that if a given interaction is linked to a master term (forms beginning with a `#`) the parameters cannot be edited here. Instead this must be done on the _Master Terms_ page of the main [Forcefield tab]({{< ref "forcefieldtab" >}}).

### Isotopologues

{{< cimage src="../speciestab_isotopologues.png" caption="Species tab showing defined isotopologues" width=90% >}}

Since Dissolve was written with primarily neutron diffraction in mind each species has a page devoted to the definition of neutron-weighted isotopologues. Each isotopologue definition specifies exactly one isotope for each atom type used in the species - mixtures or isotopic ratios are defined by mixing whole isotopologues at the point of use. 

### Analysis Sites

{{< cimage src="../speciestab_sites.png" caption="Species tab showing defined analysis sites" width=90% >}}

All defined analysis sites to be used for calculation of structural quantities are defined here, shown in a single list on the left regardless of the site type. The viewer on the right-hand side displays the centre(s) for the currently selected site, along with the accompanying axes if defined. New sites can be defined from the controls at the top of the site list, or created from an atom selection in the viewer window.