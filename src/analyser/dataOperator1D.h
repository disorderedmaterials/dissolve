// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataOperatorBase.h"
#include "math/data1D.h"

using OperateFunction1D = std::function<double(const double &, const double &, const double &)>;
class DataOperator1D : public DataOperatorBase<Data1D, OperateFunction1D>
{
    public:
    DataOperator1D(Data1D &targetData);

    /*
     * Generic Operate Functions
     */
    public:
    void operate(OperateFunction1D operateFunction) override;

    /*
     * Normalisation Functions
     */
    public:
    void normaliseByGrid() override;
    void normaliseBySphericalShell() override;
    void normaliseTo(double value = 1.0, bool absolute = true) override;
};