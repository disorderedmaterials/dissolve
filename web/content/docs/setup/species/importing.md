---
title: Importing Species
description: Importing species from other sources
weight: 2
---

### From an Existing Simulation

Dissolve can extract species definitions from existing input files, allowing quick re-use of those from your own simulations or those of others. From the main menu:

{{< action type="menu" text="Species &#8680; Import... &#8680; From Existing Simulation..." >}}

The wizard will guide you through choosing an existing input file and selecting a target species to import. The full forcefield of the species, including atom types, is also copied. Because this may cause conflicts with atom types or master terms already present in your simulation, the wizard will check for such conflicts and allows renaming of any terms.

### From XYZ coordinates

A bare species with no forcefield terms can be created from a standard XYZ file:

{{< action type="menu" text="Species &#8680; Import... &#8680; From XYZ..." >}}

Once an XYZ file has been chosen Dissolve will read the coordinates, calculate connectivity between atoms, and put the result up for editing (as if it had been drawn by hand) so that any errors can be corrected (e.g. missing or extra bonds between atoms).

### From LigParGen

A neat way of importing a species including a complete forcefield (with charges) into Dissolve is to use the [LigParGen service](http://zarbi.chem.yale.edu/ligpargen/) offered by the Jorgensen group at Yale. There you can input a structure from its [SMILES notation](https://en.wikipedia.org/wiki/Simplified_molecular-input_line-entry_system) or an existing structure file, and the service will create for you a forcefield definition based on the OPLS-AA parameter set. It's possible to find SMILES codes for most molecules online - for instance through [PubChem](https://pubchem.ncbi.nlm.nih.gov/).

After submitting your structure to the service, you'll arrive at a webpage offering you downloads of the resulting forcefield in a variety of formats. Either take the zip file which contains everything, or separately download both the OpenMM XML and Tinker XYZ files. Then, you can offer these to Dissolve:

{{< action type="menu" text="Species &#8680; Import... &#8680; From LigParGen..." >}}

The wizard will ask for these OpenMM and XYZ files as a first step - providing both will give you a complete species within Dissolve. As with the option to import species from existing simulations, a full forcefield for the species is created and the necessary checks are made for any conflicts with existing atom types or master terms.

Worth noting that the default behaviour of the LigParGen import wizard is to reduce intramolecular terms to master terms based on the similarity of parameters, as well as for atom types.
