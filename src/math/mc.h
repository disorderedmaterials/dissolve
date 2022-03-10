// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "expression/variable.h"
#include <functional>
#include <memory>
#include <vector>

class MonteCarloMinimiser
{
    using MinimiserCostFunction = std::function<double()>;

    public:
    explicit MonteCarloMinimiser(MinimiserCostFunction costFunction);

    private:
    // Maximum number of iterations to perform
    int maxIterations_{100};
    // Step size
    double stepSize_{0.1};
    // Minimum step size
    double minStepSize_{1.0e-5};
    // Acceptance memory length
    int acceptanceMemoryLength_{25};
    // Target acceptance ratio
    double targetAcceptanceRatio_{0.33};
    // Pointer to cost function
    MinimiserCostFunction costFunction_;
    // Pointers to double values to be fit
    std::vector<double *> targets_;

    private:
    // Poke supplied values into target variables
    void pokeValues(const std::vector<double> &values);
    // Calculate cost from specified values
    double cost(const std::vector<double> &alpha);

    public:
    // Set maximum number of iterations to perform
    void setMaxIterations(int maxIterations);
    // Set step size
    void setStepSize(double stepSize);
    // Return step size
    double stepSize();
    // Set minimum step size
    void setMinStepSize(double minStepSize);
    // Set acceptance memory length
    void setAcceptanceMemoryLength(int length);
    // Target acceptance ratio
    void setTargetAcceptanceRatio(double ratio);
    // Add fit target
    void addTarget(double *var);
    // Minimise target parameters
    double minimise();
};
