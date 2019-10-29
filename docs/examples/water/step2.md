---
title: Step 2 - Set up a Configuration
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 2. Set up a Configuration

From the _Configuration_{: .text-green-100} menu select _Create...→Simple random mix_{: .text-green-100} and choose your water species as the only component. The default density parameter (`rho`) defined in the generator for the configuration is 0.1 atoms/Angstrom**3, which just so happens to be the density for water at 298 K. All we need to adjust is the number of molecules in the system in the `AddSpecies` node for the water molecule. Change this to 1000, and then _Regenerate_{: .text-green-100} the configuration to get a box size of just over 31 Angstroms side length.

Finally - don't forget to change the _Temperature_{: .text-green-100} to 298 K.

![Water box](waterbox.png){: .img-centre}

[Previous Step](step1.md){: .btn }   [Next Step](step3.md){: .btn .right}
