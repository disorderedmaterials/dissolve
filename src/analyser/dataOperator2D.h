// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataOperatorBase.h"
#include "math/data2D.h"

using NormalisationFunction2D =
    std::function<double(const double &, const double &, const double &, const double &, const double &)>;
class DataOperator2D : public DataOperatorBase<Data2D, NormalisationFunction2D>
{
    public:
    DataOperator2D(Data2D &targetData);

    /*
     * Normalisation Functions
     */
    public:
    void normalise(NormalisationFunction2D normalisationFunction) override;
    void normaliseByGrid() override;
    void normaliseBySphericalShell() override;
    void normaliseTo(double value = 1.0, bool absolute = true) override;
};