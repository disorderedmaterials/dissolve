// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataNormaliserBase.h"
#include "math/data1D.h"
#include <string_view>

class DataNormaliser1D : public DataNormaliserBase<Data1D>
{
    public:
    DataNormaliser1D(Data1D &targetData);

    /*
     * Normalisation functions
     */
    public:
    void normaliseByNumberDensity(double population, Configuration *targetConfiguration) override;
    void normaliseBySitePopulation(double population) override;
    void normaliseBySphericalShell() override;
    void normaliseByValue(double value = 1.0, bool absolute = true) override;
    void normaliseByExpression(std::string_view expressionString) override;
    void normaliseByGrid() override;
};