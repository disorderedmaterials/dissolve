// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataNormaliserBase.h"
#include "math/data1D.h"
#include <string_view>

using NormalisationFunction1D = std::function<double(const double &, const double &)>;
class DataNormaliser1D : public DataNormaliserBase<Data1D, NormalisationFunction1D>
{
    public:
    DataNormaliser1D(Data1D &targetData);

    /*
     * Normalisation functions
     */
    public:
    void normalise(NormalisationFunction1D normalisationFunction) override;
    void normaliseByGrid() override;
    void normaliseBySphericalShell() override;
    void normaliseTo(double value = 1.0, bool absolute = true) override;
};