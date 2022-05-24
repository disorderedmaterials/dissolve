---
title: Beginner's Guide - Argon
description: Step-by-step set up of a simple simulation, recommended for first-time users of the code
type: docs
weight: 1
---

{{< label color="blue" icon="fa-clock" text="20 minutes" >}}

## Summary

Liquid argon isn't a particularly exciting system, but it has been measured experimentally by neutron scattering, and it serves as an excellent example to introduce some of the basic workflows of Dissolve. This example will show you how to set up a complete working simulation from scratch, with the purpose to simulate liquid argon at 85 K and compare the simulation to reference neutron data from ["Structure Factor and Radial Distribution Function for Liquid Argon at 85 °K"](https://journals.aps.org/pra/abstract/10.1103/PhysRevA.7.2130), J. L. Yarnell, M. J. Katz, R. G. Wenzel, and S. H. Koenig, _Phys. Rev. A_ **7**, 2130.

## Preparation

Download the example data as a {{< exampledatalink linkText="zip" urlSuffix="zip" >}} or {{< exampledatalink linkText="tar.gz" urlSuffix="tar.gz" >}} and unpack it if you haven't done so already.  Alternatively, download the data files listed below and save them to a location of your choice.

- Neutron data file: [yarnell.sq](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/argon/data/yarnell.sq)

{{< action type="menu" text="File &#8680; New" />}}
{{< action type="menu" text="File &#8680; Save As..." />}}
{{< step text="Save your input file under a sensible name in same directory as the example data" />}}


{{< button pos="left" text="Let's begin!" path="step1">}}
