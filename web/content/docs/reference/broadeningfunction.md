---
title: Broadening Functions
description: Available analytic broadening functions, for use in e.g. Fourier transforms
---

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`None`|--|No broadening function|
|`Gaussian`|1: `FWHM`|Un-normalised Gaussian with no prefactor $$ f(x) = \exp\left(-\frac{x^2}{2c^2}\right), c = \frac{\textrm{FWHM}}{2 \sqrt{2 \ln(2)}} $$ |
|`ScaledGaussian`|1: `A`</br>2: `FWHM`|Un-normalised Gaussian with prefactor $$ f(x) = A\exp\left(-\frac{x^2}{2c^2}\right), c = \frac{\textrm{FWHM}}{2 \sqrt{2 \ln(2)}} $$ |
|`OmegaDependentGaussian`|1: `FWHM`|Un-normalised Gaussian with variable FWHM and no prefactor - note that the parameter $\omega$ is set implicitly by the context using the broadening function $$ f(x) = \exp\left(-\frac{x^2}{2(c\omega)^2}\right), c = \frac{\textrm{FWHM}}{2 \sqrt{2 \ln(2)}} $$ |
|`GaussianC2`|1: `FWHM1`</br>2: `FWHM2`|Un-normalised Gaussian with constant and variable FWHM components and no prefactor - note that the parameter $\omega$ is set implicitly by the context using the broadening function $$ f(x) = \exp\left(-\frac{x^2}{2(c_1 + c_2\omega)^2}\right), c_n = \frac{\textrm{FWHM}_n}{2 \sqrt{2 \ln(2)}} $$ |
