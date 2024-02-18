// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataNormaliserBase.h"
#include "math/data2D.h"
#include <string_view>

class DataNormaliser2D : public DataNormaliserBase<Data2D>
{
    public:
    DataNormaliser2D(Data2D &targetData);

    /*
     * Normalisation functions
     */
    public:
    void normaliseBySphericalShell() override;
    void normaliseByExpression(std::string_view expressionString) override;
    void normaliseByGrid() override;
};