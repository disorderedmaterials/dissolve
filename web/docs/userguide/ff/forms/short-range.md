---
title: Short Range Potentials
description: Functional forms for short range potentials
---

Below are listed the interaction forms available for short range non-bonded interactions between atoms, with energy and force equations provided in full. Parameters are listed in the input order expected by Dissolve.

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`LJ`|`$\epsilon$`</br>`$\sigma$`|Standard Lennard-Jones 12-6 potential. $$ E = 4 \epsilon \left( \left(\frac{\sigma}{r}\right)^{12} - \left(\frac{\sigma}{r}\right)^6 \right) $$ $$ F = -48 \epsilon \left( \frac{\sigma^{12}}{r^{13}} - \frac{1}{2} \frac{\sigma^6}{r^7} \right) $$  Cross-terms between atom types with this form are generated via the Lorenz-Berthelot combination rules: $$ \sigma_{ij} = \frac{\sigma_i + \sigma_j}{2}, \epsilon_{ij} = \sqrt{\epsilon_i \epsilon_j} $$ |
|`LJGeom`|`$\epsilon$`</br>`$\sigma$`|Standard Lennard-Jones 12-6 potential as defined for `LJ`, but utilising purely geometric rules when generating cross-terms: $$ \sigma_{ij} = \sqrt{\sigma_i \sigma_j}, \epsilon_{ij} = \sqrt{\epsilon_i \epsilon_j} $$ |
