// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/ft.h"
#include "math/data1D.h"
#include "templates/algorithms.h"
#include "templates/parallelDefs.h"
#include <algorithm>
#include <functional>
#include <numeric>
#include <vector>

namespace Fourier
{
// Perform Fourier sine transform of current distribution function, over range specified, and with specified broadening
// function, modification function, and window applied (if requested)
bool sineFT(Data1D &data, double normFactor, double wMin, double wStep, double wMax, WindowFunction windowFunction,
            const Function1DWrapper &broadening)
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
    dissolve::transform(ParallelPolicies::par, newX.begin(), newX.end(), newX.begin(),
                        [wMin, wStep](const auto idx) { return wMin + idx * wStep; });

    std::vector<double> newY(newX.size());

    std::vector<double> deltas(x.size()), product(x.size() - 1);
    std::adjacent_difference(x.begin(), x.end(), deltas.begin());
    dissolve::transform(ParallelPolicies::par, x.begin(), x.end() - 1, y.begin(), product.begin(),
                        [](auto x, auto y) { return x * y; });
    dissolve::transform(ParallelPolicies::par, product.begin(), product.end(), deltas.begin() + 1, product.begin(),
                        std::multiplies());

    // Perform Fourier sine transform, apply general and omega-dependent broadening, as well as window function
    dissolve::transform(ParallelPolicies::par, newX.begin(), newX.end(), newY.begin(),
                        [normFactor, &product, &x, &windowFunction, &broadening](const auto omega)
                        {
                            double ft = 0.0;

                            if (omega > 0.0)
                                ft = std::transform_reduce(
                                    product.begin(), product.end(), x.begin(), 0.0, std::plus(),
                                    [omega, &windowFunction, &broadening](const auto r, const auto x)
                                    { return r * sin(x * omega) * windowFunction.y(x, omega) * broadening.yFT(x, omega); });
                            else
                                ft = std::transform_reduce(product.begin(), product.end(), x.begin(), 0.0, std::plus(),
                                                           [omega, &windowFunction, &broadening](const auto r, const auto x) {
                                                               return r * windowFunction.y(x, omega) * broadening.yFT(x, omega);
                                                           });

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
