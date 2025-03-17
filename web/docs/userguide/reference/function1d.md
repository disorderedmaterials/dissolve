---
title: One-Dimensional Functions
description: Analytic one-dimensional functions
---

Function available for use when, e.g., applying broadening to calculated structure factors, radial distribution functions etc.

|Keyword|Parameters|Derivative<sup>1</sup>|FT<sup>2</sup>|DKN<sup>3</sup>|Description|
|:-----:|:--------:|:--------------------:|:------------:|:-------------:|-----------|
|`None`|--||&check;|&check;|No function - returns zero.|
|`Gaussian`|`FWHM`||&check;|&check;|Un-normalised Gaussian with no prefactor $$ f(x) = \exp\left(-\frac{x^2}{2c^2}\right), c = \frac{\textrm{FWHM}}{2 \sqrt{2 \ln(2)}} $$ |
|`ScaledGaussian`|`A`</br>`FWHM`||&check;|&check;|Un-normalised Gaussian with prefactor $$ f(x) = A\exp\left(-\frac{x^2}{2c^2}\right), c = \frac{\textrm{FWHM}}{2 \sqrt{2 \ln(2)}} $$ |
|`OmegaDependentGaussian`|`FWHM`||&check;|&check;|Un-normalised Gaussian with variable FWHM and no prefactor - note that the parameter $\omega$ is set implicitly by the context using the broadening function $$ f(x) = \exp\left(-\frac{x^2}{2(c\omega)^2}\right), c = \frac{\textrm{FWHM}}{2 \sqrt{2 \ln(2)}} $$ |
|`GaussianC2`|`FWHM1`</br>`FWHM2`||&check;|&check;|Un-normalised Gaussian with constant and variable FWHM components and no prefactor - note that the parameter $\omega$ is set implicitly by the context using the broadening function $$ f(x) = \exp\left(-\frac{x^2}{2(c_1 + c_2\omega)^2}\right), c_n = \frac{\textrm{FWHM}_n}{2 \sqrt{2 \ln(2)}} $$ |
|`LennardJones126`|`$\epsilon$`</br>`$\sigma$`|&check;|||Lennard-Jones 12-6 short-range potential $$ f(x) = 4\epsilon \left( \left( \frac{\sigma}{x} \right)^{12} - \left( \frac{\sigma}{x} \right)^6 \right) $$|
|`Buckingham`|`A`</br>`B`</br>`C`|&check;|||Buckingham short-range potential $$ f(x) = A \exp\left(-B x\right) $$|
|`GaussianPotential`|`A`</br>`fwhm`</br>`$x_0$`|&check;|||Gaussian potential centred at $x_0$, intended to be used as a potential override.  $$ f(x) = A\exp\left(-\frac{\left(x-x_0\right)^2}{2c^2}\right), c = \frac{\textrm{FWHM}}{2 \sqrt{2 \ln(2)}} $$ |
|`Harmonic`|`k`|&check;|||Simple harmonic well potential$$ E_{ij} = \frac{1}{2} k r^2 $$ $$ F_{ij} = -k r $$ |

<sup>1</sup>Whether the first derivative of the function is defined

<sup>2</sup>Whether an analytic Fourier transform is defined

<sup>3</sup>Whether discrete kernel normalisation is defined (function sums to 1.0)
