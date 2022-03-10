/*
 */

#pragma once

#include "expression/variable.h"
#include <functional>
#include <memory>
#include <vector>

class MonteCarloMinimiser
{
    using MinimiserCostFunction = std::function<double()>;

    public:
    MonteCarloMinimiser(MinimiserCostFunction costFunction);

    private:
    // Maximum number of iterations to perform
    int maxIterations_;
    // Step size
    double stepSize_;
    // Minimum step size
    double minStepSize_;
    // Parameter smoothing frequency (required number of accepted moves)
    int parameterSmoothingFrequency_;
    // Parameter smoothing control (Kolmogorov-Zurbenko)
    int parameterSmoothingK_, parameterSmoothingM_;
    // Acceptance memory length
    int acceptanceMemoryLength_;
    // Target acceptance ratio
    double targetAcceptanceRatio_;
    // Pointer to cost function
    MinimiserCostFunction costFunction_;
    // Pointers to double values to be fit
    std::vector<double *> targets_;

    private:
    void pokeValues(const std::vector<double> &values);

    // Calculate cost from specified values, including contributions from any supplied limits
    double cost(const std::vector<double> &alpha);

    // Smooth current parameter set
    void smoothParameters(std::vector<double> &values);

    public:
    // Set maximum number of iterations to perform
    void setMaxIterations(int maxIterations);
    // Set step size
    void setStepSize(double stepSize);
    // Return step size
    double stepSize();
    // Set minimum step size
    void setMinStepSize(double minStepSize);
    // Enable parameter smoothing
    void enableParameterSmoothing(int everyNAccepted, int smoothK, int smoothM);

    // Set acceptance memory length
    void setAcceptanceMemoryLength(int length);
    // Target acceptance ratio
    void setTargetAcceptanceRatio(double ratio);
    // Perform minimisation
    double execute(std::vector<double> &values);

    // Minimise target parameters
    double minimise();

    // Add fit target
    void addTarget(double *var);
};
