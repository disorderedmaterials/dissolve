// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataNormaliserBase.h"
#include "math/data2D.h"
#include <string_view>

using NormalisationFunction2D =
    std::function<double(const double &, const double &, const double &, const double &, const double &)>;
class DataNormaliser2D : public DataNormaliserBase<Data2D, NormalisationFunction2D>
{
    public:
    DataNormaliser2D(Data2D &targetData);

    /*
     * Normalisation functions
     */
    public:
    void normalise(NormalisationFunction2D normalisationFunction) override;
    void normaliseByGrid() override;
    void normaliseBySphericalShell() override;
    void normaliseTo(double value = 1.0, bool absolute = true) override;
};