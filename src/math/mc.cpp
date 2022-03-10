// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "math/mc.h"
#include "base/messenger.h"
#include "math/mathfunc.h"
#include <numeric>

MonteCarloMinimiser::MonteCarloMinimiser(MinimiserCostFunction costFunction) : costFunction_(std::move(costFunction)) {}

// Poke supplied values into target variables
void MonteCarloMinimiser::pokeValues(const std::vector<double> &values)
{
    for (auto n = 0; n < targets_.size(); ++n)
        (*targets_[n]) = values[n];
}

// Calculate cost from specified values
double MonteCarloMinimiser::cost(const std::vector<double> &alpha)
{
    // Poke values into targets before calling cost function
    pokeValues(alpha);

    return costFunction_();
}

// Set maximum number of iterations to perform
void MonteCarloMinimiser::setMaxIterations(int maxIterations) { maxIterations_ = maxIterations; }

// Set step size
void MonteCarloMinimiser::setStepSize(double stepSize) { stepSize_ = stepSize; }

// Return step size
double MonteCarloMinimiser::stepSize() { return stepSize_; }

// Set minimum step size
void MonteCarloMinimiser::setMinStepSize(double minStepSize) { minStepSize_ = minStepSize; }

// Set acceptance memory length
void MonteCarloMinimiser::setAcceptanceMemoryLength(int length) { acceptanceMemoryLength_ = length; }

// Target acceptance ratio
void MonteCarloMinimiser::setTargetAcceptanceRatio(double ratio) { targetAcceptanceRatio_ = ratio; }

// Add fit target, with limits specified
void MonteCarloMinimiser::addTarget(double *var) { targets_.push_back(var); }

// Minimise target parameters
double MonteCarloMinimiser::minimise()
{
    // Check for zero variable parameters
    if (targets_.size() == 0)
    {
        Messenger::warn("No variables specified for fitting, so nothing to do.\n");
        return 0.0;
    }

    // Create a local array of values to pass to the fitting routine
    std::vector<double> values(targets_.size());
    std::transform(targets_.begin(), targets_.end(), values.begin(), [](auto *target) { return *target; });

    // Get initial error of input parameters
    auto currentError = cost(values);
    Messenger::printVerbose("MonteCarloMinimiser<T>::minimise() - Initial error = {}\n", currentError);

    double trialError;
    std::vector<double> trialValues;
    std::vector<int> accepts;
    bool accepted;
    int smoothingNAccepted = 0;

    // Outer loop
    for (auto iter = 0; iter < maxIterations_; ++iter)
    {
        // Copy current best alpha
        trialValues = values;

        // Perform a Monte Carlo move on a random parameter
        auto i = int(trialValues.size() * DissolveMath::random());
        if (i >= trialValues.size())
            i = trialValues.size() - 1;

        if (fabs(trialValues[i]) < 1.0e-8)
            trialValues[i] += DissolveMath::randomPlusMinusOne() * 0.01 * stepSize_;
        else
            trialValues[i] += DissolveMath::randomPlusMinusOne() * trialValues[i] * stepSize_;

        // Get error for the new parameters, and store if improved
        trialError = cost(trialValues);
        accepted = trialError < currentError;
        if (accepted)
        {
            values[i] = trialValues[i];
            currentError = trialError;

            ++smoothingNAccepted;
        }
        else
            trialValues[i] = values[i];

        // Accumulate acceptance memory
        accepts.push_back(accepted);

        // If we have enough acceptance memory, adjust the step size
        if (accepts.size() == acceptanceMemoryLength_)
        {
            double ratio =
                targetAcceptanceRatio_ / (std::accumulate(accepts.begin(), accepts.end(), 0.0) / acceptanceMemoryLength_);
            stepSize_ /= ratio;
            if (stepSize_ < minStepSize_)
                stepSize_ = minStepSize_;

            accepts.clear();
        }

        if (maxIterations_ < 10 || (iter % (maxIterations_ / 10) == 0))
            Messenger::printVerbose("MonteCarloMinimiser<T>::minimise() - Iteration {} error = {}, stepSize = {}\n", iter + 1,
                                    currentError, stepSize_);
    }

    // Set optimised values back into their original variables
    pokeValues(values);

    return currentError;
}
