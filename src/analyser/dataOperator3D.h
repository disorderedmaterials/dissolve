// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataOperatorBase.h"
#include "math/data3D.h"

using OperateFunction3D = std::function<double(const double &, const double &, const double &, const double &, const double &,
                                               const double &, const double &)>;
class DataOperator3D : public DataOperatorBase<Data3D, OperateFunction3D>
{
    public:
    DataOperator3D(Data3D &targetData);

    /*
     * Generic Operate Functions
     */
    public:
    void operate(OperateFunction3D operateFunction) override;

    /*
     * Normalisation Functions
     */
    public:
    void normaliseByGrid() override;
    void normaliseBySphericalShell() override;
    void normaliseTo(double value = 1.0, bool absolute = true) override;
};