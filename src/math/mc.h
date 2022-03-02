/*
 */

#pragma once

#include "expression/variable.h"
#include <functional>
#include <memory>

class MonteCarloMinimiser
{
    using MinimiserCostFunction = std::function<double(const std::vector<double> &)>;

    public:
    MonteCarloMinimiser(MinimiserCostFunction costFunction, bool pokeBeforeCost = false);

    private:
    // Where thos poke values into targets before calling the cost function
    bool pokeBeforeCost_;
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
    // Whether maximum limits have been set for targets
    std::vector<bool> maximumLimit_;
    // Whether minimum limits have been set for targets
    std::vector<bool> minimumLimit_;
    // Scaling factor for penalties incurred when outside of allowable limit
    const double penaltyFactor_ = 1e5;
    // Minimum limiting values for targets
    std::vector<double> minimumValue_;
    // Maximum limiting values for targets
    std::vector<double> maximumValue_;
    // Integer power of penalty function when outside allowable limit
    const int penaltyPower_ = 2;

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

    // Add pointer as fit target, with limits specified
    void addTarget(double *var, bool minLimit = false, double minValue = 0.0, bool maxLimit = false, double maxValue = 0.0);

    // Add reference as fit target, with limits specified
    void addTarget(double &var, bool minLimit = false, double minValue = 0.0, bool maxLimit = false, double maxValue = 0.0);

    void addTarget(const std::shared_ptr<ExpressionVariable> &var, bool minLimit = false, double minValue = 0.0,
                   bool maxLimit = false, double maxValue = 0.0);
};
