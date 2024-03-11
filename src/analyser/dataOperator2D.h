// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataOperatorBase.h"
#include "math/data2D.h"

using OperateFunction2D = std::function<double(const double &, const double &, const double &, const double &, const double &)>;
class DataOperator2D : public DataOperatorBase<Data2D, OperateFunction2D>
{
    public:
    DataOperator2D(Data2D &targetData);

    /*
     * Generic Operate Functions
     */
    public:
    void operate(OperateFunction2D operateFunction) override;

    /*
     * Normalisation Functions
     */
    public:
    void normaliseByGrid() override;
    void normaliseBySphericalShell() override;
    void normaliseTo(double value = 1.0, bool absolute = true) override;
};