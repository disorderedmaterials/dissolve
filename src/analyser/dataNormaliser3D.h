// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataNormaliserBase.h"
#include "math/data3D.h"
#include <string_view>

class DataNormaliser3D : public DataNormaliserBase<Data3D>
{
    public:
    DataNormaliser3D(Data3D &targetData);

    /*
     * Normalisation functions
     */
    public:
    void normaliseBySphericalShell() override;
    void normaliseTo(double value = 1.0, bool absolute = true) override;
    void normaliseByExpression(std::string_view expressionString) override;
    void normaliseByGrid() override;
};