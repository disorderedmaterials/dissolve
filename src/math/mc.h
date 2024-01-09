// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "expression/variable.h"
#include <functional>
#include <memory>
#include <vector>

class MonteCarloMinimiser
{
    using MinimiserCostFunction = std::function<double()>;
    using MinimiserSamplingFunction = std::function<void(std::vector<double> &)>;

    public:
    explicit MonteCarloMinimiser(MinimiserCostFunction costFunction, MinimiserSamplingFunction samplingFunction = {});

    private:
    // Maximum number of iterations to perform
    int maxIterations_{100};
    // Step size
    double stepSize_{0.1};
    // Target acceptance ratio
    double targetAcceptanceRatio_{0.33};
    // Pointer to cost function
    MinimiserCostFunction costFunction_;
    // Frequency at which sampling function will be called
    int samplingFrequency_{0};
    // Pointer to periodic sampling function
    MinimiserSamplingFunction samplingFunction_;
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
    double stepSize() const;
    // Target acceptance ratio
    void setTargetAcceptanceRatio(double ratio);
    // Set sampling frequency
    void setSamplingFrequency(int frequency);
    // Add fit target
    void addTarget(double *var);
    // Minimise target parameters
    double minimise();
};
