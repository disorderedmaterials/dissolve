// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/ft.h"
#include "math/data1d.h"
#include "templates/parallel_defs.h"
#include <algorithm>
#include <functional>
#include <numeric>
#include <vector>

namespace Fourier
{
// Perform Fourier sine transform of current distribution function, over range specified, and with specified broadening
// function, modification function, and window applied (if requested)
bool sineFT(Data1D &data, double normFactor, double wMin, double wStep, double wMax, WindowFunction windowFunction,
            const Functions::Function1DWrapper &broadening)
{
    /*
     * Perform sine Fourier transform of current data. Function has no notion of forward or backwards transforms -
     * normalisation and broadening functions must be suitable for the required purpose. Broadening functions are applied to
     * the transformed function utilising convolution theorem:
     *
     * 	f(x) and g(x) are the original functions, while F(q) and G(q) are their Fourier transforms.
     * 	Pointwise multiplication (.) in one domain equals convolution (*) in the other:
     *
     * 	FT[ f(x) * g(x) ] = F(q) . G(q)
     * 	FT[ f(x) . g(x) ] = F(q) * G(q)
     *
     * Since the ultimate goal of this function is to generate the broadened FT of the input data (with the broadening
     * applied to the transformed data, rather than applied to the input data and then transformed) we require the first
     * case listed above. The quantity we want is the pointwise multiplication of the FT of the input data with the
     * broadening functions given, so we can simply perform the convolution of the input data with the *FT* of the
     * broadening functions, and FT the result.
     */

    // Set up window function for the present data
    windowFunction.setUp(data);

    // Grab x and y arrays
    const auto &x = data.xAxis();
    const auto &y = data.values();

    // Create working arrays
    std::vector<double> newX((wMax - wMin) / wStep);
    std::iota(newX.begin(), newX.end(), 0);
    std::transform(ParallelPolicies::par, newX.begin(), newX.end(), newX.begin(),
                   [wMin, wStep](const auto idx) { return wMin + idx * wStep; });

    // auto omega = wMin;
    // for (auto &xval : newX)
    // {
    //     xval = omega;
    //     omega += wStep;
    // }

    std::vector<double> newY(newX.size());

    // Perform Fourier sine transform, apply general and omega-dependent broadening, as well as window function
    std::transform(ParallelPolicies::par, newX.begin(), newX.end(), newY.begin(),
                   [normFactor, &x, &y, &windowFunction, &broadening](const auto omega) {
                       double ft = 0.0;
                       const auto nX = x.size();
                       std::vector<double> deltas(nX), windows(nX - 1), broadens(nX - 1), result(nX - 1);
                       std::adjacent_difference(x.begin(), x.end(), deltas.begin());
                       std::transform(ParallelPolicies::par, x.begin(), x.end() - 1, windows.begin(),
                                      [omega, &windowFunction](const auto X) { return windowFunction.y(X, omega); });
                       std::transform(ParallelPolicies::par, x.begin(), x.end() - 1, broadens.begin(),
                                      [omega, &broadening](const auto X) { return broadening.yFT(X, omega); });
                       std::transform(ParallelPolicies::par, x.begin(), x.end() - 1, y.begin(), result.begin(),
                                      [](auto x, auto y) { return x * y; });
                       std::transform(ParallelPolicies::par, result.begin(), result.end(), windows.begin(), result.begin(),
                                      std::multiplies());
                       std::transform(ParallelPolicies::par, result.begin(), result.end(), broadens.begin(), result.begin(),
                                      std::multiplies());
                       std::transform(ParallelPolicies::par, result.begin(), result.end(), deltas.begin() + 1, result.begin(),
                                      std::multiplies());
                       if (omega > 0.0)
                           std::transform(ParallelPolicies::par, result.begin(), result.end(), x.begin(), result.begin(),
                                          [omega](const auto r, const auto x) { return sin(x * omega) * r; });
                       ft = std::reduce(result.begin(), result.end(), 0.0);

                       // Normalise w.r.t. omega
                       if (omega > 0.0)
                           ft /= omega;

                       // Add point
                       return ft * normFactor;
                   });

    // Transfer working arrays to this object
    data.xAxis() = newX;
    data.values() = newY;

    return true;
}
} // namespace Fourier
