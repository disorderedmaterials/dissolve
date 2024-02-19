// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataNormaliserBase.h"
#include "math/data3D.h"
#include <string_view>

<<<<<<< HEAD
class DataNormaliser3D : public DataNormaliserBase<Data3D>
=======
using NormalisationFunction3D = std::function<double(const double &, const double &, const double &, const double &)>;
class DataNormaliser3D : public DataNormaliserBase<Data3D, NormalisationFunction3D>
>>>>>>> develop
{
    public:
    DataNormaliser3D(Data3D &targetData);

    /*
     * Normalisation functions
     */
    public:
<<<<<<< HEAD
    void normaliseBySphericalShell() override;
    void normaliseByExpression(std::string_view expressionString) override;
    void normaliseByGrid() override;
=======
    void normalise(NormalisationFunction3D normalisationFunction) override;
    void normaliseByGrid() override;
    void normaliseBySphericalShell() override;
    void normaliseTo(double value = 1.0, bool absolute = true) override;
>>>>>>> develop
};