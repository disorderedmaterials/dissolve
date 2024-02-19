// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
<<<<<<< HEAD
#include "math/integrator.h"
#include <string_view>

template <typename DataND> class DataNormaliserBase
=======
#include <string_view>

template <typename DataND, typename NormalisationFunction> class DataNormaliserBase
>>>>>>> develop
{

    public:
    DataNormaliserBase(DataND &targetData) : targetData_(targetData) {}

<<<<<<< HEAD
    protected:
    DataND &targetData_;

    public:
    void normaliseByNumberDensity(double population, Configuration *targetConfiguration)
    {
        targetData_ /= (population / targetConfiguration->box()->volume());
    }
    void normaliseBySitePopulation(double population) { targetData_ /= population; }
    void normaliseByValue(double value = 1.0, bool absolute = true)
    {
        auto sum = absolute ? Integrator::absSum(targetData_) : Integrator::sum(targetData_);
        targetData_ /= sum;
        targetData_ *= value;
    }
    virtual void normaliseBySphericalShell() = 0;
    virtual void normaliseByExpression(std::string_view expressionString) = 0;
    virtual void normaliseByGrid() = 0;
=======
    /*
     * Targets
     */
    protected:
    DataND &targetData_;

    /*
     * Normalisation functions
     */
    public:
    void normaliseDivide(double divisor) { targetData_ /= divisor; }
    virtual void normalise(NormalisationFunction normalisationFunction) = 0;
    virtual void normaliseByGrid() = 0;
    virtual void normaliseBySphericalShell() = 0;
    virtual void normaliseTo(double value = 1.0, bool absolute = true) = 0;
>>>>>>> develop
};