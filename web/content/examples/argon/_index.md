---
title: Liquid Argon
type: docs
---

{{< label color="blue" icon="fa-clock" text="20 minute read" >}}

## Summary

Liquid argon isn't a particularly exciting system, but it has been measured experimentally by neutron scattering, and it serves as an excellent example to introduce some of the basic workflows of Dissolve. This example will show you how to set up a complete working simulation from scratch, with the purpose to simulate liquid argon at 85 K and compare the simulation to reference neutron data from ["Structure Factor and Radial Distribution Function for Liquid Argon at 85 °K"](https://journals.aps.org/pra/abstract/10.1103/PhysRevA.7.2130), J. L. Yarnell, M. J. Katz, R. G. Wenzel, and S. H. Koenig, _Phys. Rev. A_ **7**, 2130.

## Preparation

Download the example data as a {{< exampledatalink linkText="zip" urlSuffix="zip" >}} or {{< exampledatalink linkText="tar.gz" urlSuffix="tar.gz" >}} and unpack it if you haven't done so already.


{{< action type="dissolve" text="Open the Dissolve GUI" >}}
{{< step text="Choose **Empty Simulation** from the _Create_ section" >}} 
{{< action type="menu" text="Save As..." >}}
{{< step text="Save your own input file under a sensible name in the `argon` directory of the example data" >}}

Alternatively, download the data files listed below and save them to a location of your choice.

- Neutron data file: [yarnell.sq](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/argon/data/yarnell.sq)


{{< button pos="left" text="Let's begin!" path="step1">}}
