---
title: One-Dimensional Functions
description: Analytic one-dimensional functions
---

Function available for use when, e.g., applying broadening to calculated structure factors, radial distribution functions etc.

|Keyword|Parameters|FT<sup>1</sup>|DKN<sup>2</sup>|Description|
|:-----:|:--------:|:------------:|:-------------:|-----------|
|`None`|--|&check;|&check;|No broadening function|
|`Gaussian`|`FWHM`|&check;|&check;|Un-normalised Gaussian with no prefactor $$ f(x) = \exp\left(-\frac{x^2}{2c^2}\right), c = \frac{\textrm{FWHM}}{2 \sqrt{2 \ln(2)}} $$ |
|`ScaledGaussian`|`A`</br>`FWHM`|&check;|&check;|Un-normalised Gaussian with prefactor $$ f(x) = A\exp\left(-\frac{x^2}{2c^2}\right), c = \frac{\textrm{FWHM}}{2 \sqrt{2 \ln(2)}} $$ |
|`OmegaDependentGaussian`|`FWHM`|&check;|&check;|Un-normalised Gaussian with variable FWHM and no prefactor - note that the parameter $\omega$ is set implicitly by the context using the broadening function $$ f(x) = \exp\left(-\frac{x^2}{2(c\omega)^2}\right), c = \frac{\textrm{FWHM}}{2 \sqrt{2 \ln(2)}} $$ |
|`GaussianC2`|`FWHM1`</br>`FWHM2`|&check;|&check;|Un-normalised Gaussian with constant and variable FWHM components and no prefactor - note that the parameter $\omega$ is set implicitly by the context using the broadening function $$ f(x) = \exp\left(-\frac{x^2}{2(c_1 + c_2\omega)^2}\right), c_n = \frac{\textrm{FWHM}_n}{2 \sqrt{2 \ln(2)}} $$ |

<sup>1</sup>Whether an analytic Fourier transform is defined

<sup>2</sup>Whether discrete kernel normalisation is defined (function sums to 1.0)
