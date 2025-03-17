---
title: Species Tab
description: Species tabs
weight: 2
---

A Species tab exposes all aspects of a single species defined in the simulation. Species can be created from the _Species_ menu, and can be removed from the simulation by clicking the close button associated with the tab. Various options to manipulate the currently-visible species (tab) can also be found on the _Species_ menu.

{{< cimage src="../speciestab.png" caption="Species tab showing the geometry of the defined molecule" width=90% >}}

The structure tab shows the atom geometry and connectivity as a fairly standard stick or ball and stick model. While you can rotate and zoom the view, and even minimise the geometry according to the currently-defined forcefield terms for the species, the atoms and their connectivity cannot be directly edited or added to. In other words, once a species is defined its atomic contents and connectivity are fixed for all time.

Along the top-right of the tab are a series of at-a-glance indicators displaying the empirical formula, calculated total charges from atom types and species atoms (for which non-zero values will be highlighted), and any defined periodic box and its density if available. Detailed information associated to the species can be found on the left-hand side of the tab, where a stack of panels show the following data.

#### Atoms

The _Atoms_ panel shows an explicit table of atoms and their coordinates, as well as their assigned atom types and charges, all of which can be directly edited. Note that these charges are the _Species Atom_ charges referred to in the _Charge Source_ section of the [forcefield tab]({{< ref "forcefieldtab" >}}).

A summary of the number of _Intramolecular Terms_ sits underneath the atoms list, but the {{< gui-button text="View/Edit" >}} button gives access to a separate dialog containing details on all of the various terms. Note that this may only be the bond (connectivity) terms if a forcefield has not been applied. Since this information is either fixed in the species (bonds) or determined by Dissolve (angles, torsions, and impropers, the latter from the applied forcefield) terms may not be added or deleted, but the functional form and parameters may be freely changed. Note that if a given interaction is linked to a master term (forms beginning with a `#`) the parameters cannot be edited here. Instead this must be done on the _Master Terms_ page of the main [Forcefield tab]({{< ref "forcefieldtab" >}}).

{{< cimage src="../speciestab_intramolecular.png" caption="Intramolecular term dialog" width=50% >}}

#### Isotopologues


Since Dissolve was written with primarily neutron diffraction in mind each species has a panel devoted to the definition of neutron-weighted isotopologues. Each isotopologue definition specifies exactly one isotope for each atom type used in the species - mixtures or isotopic ratios are defined by mixing whole isotopologues at the point of use. 

#### Sites

{{< cimage src="../speciestab_sites.png" caption="Species tab showing defined analysis sites" width=90% >}}

All defined analysis sites to be used for calculation of structural quantities are defined here, shown in a single list on the left regardless of the site type. The viewer on the right-hand side displays the centre(s) for the currently selected site, along with the accompanying axes if defined. New sites are defined by selecting atoms in the viewer and selecting the relevant option in the right-click context menu. See [defining analysis sites]({{< ref analysissites >}}) for more details.