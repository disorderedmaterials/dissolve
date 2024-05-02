// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/mc.h"
#include "base/messenger.h"
#include "math/mathFunc.h"
#include <numeric>

MonteCarloMinimiser::MonteCarloMinimiser(MinimiserCostFunction costFunction, MinimiserSamplingFunction samplingFunction)
    : costFunction_(std::move(costFunction)), samplingFunction_(std::move(samplingFunction))
{
}

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
double MonteCarloMinimiser::stepSize() const { return stepSize_; }

// Target acceptance ratio
void MonteCarloMinimiser::setTargetAcceptanceRatio(double ratio) { targetAcceptanceRatio_ = ratio; }

// Set sampling frequency
void MonteCarloMinimiser::setSamplingFrequency(int frequency) { samplingFrequency_ = frequency; }

// Add fit target, with limits specified
void MonteCarloMinimiser::addTarget(double *var) { targets_.push_back(var); }

// Minimise target parameters
double MonteCarloMinimiser::minimise()
{
    // Check for zero variable parameters
    if (targets_.empty())
    {
        Messenger::warn("No variables specified for fitting, so nothing to do.\n");
        return 0.0;
    }

    // Create a local array of values to pass to the fitting routine
    std::vector<double> values(targets_.size());
    std::transform(targets_.begin(), targets_.end(), values.begin(), [](auto *target) { return *target; });

    // Get initial error of input parameters
    auto currentError = cost(values);
    Messenger::printVerbose("MonteCarloMinimiser::minimise() - Initial error = {}\n", currentError);

    double trialError;
    std::vector<double> trialValues(values);
    auto nAccepted = 0;

    // Outer loop
    for (auto iter = 0; iter < maxIterations_; ++iter)
    {
        // Generate a set of trial values
        std::transform(values.begin(), values.end(), trialValues.begin(),
                       [&](const auto x) { return x + DissolveMath::randomPlusMinusOne() * stepSize_; });

        // Get error for the new parameters, and store if improved
        trialError = cost(trialValues);
        if (trialError < currentError)
        {
            values = trialValues;
            currentError = trialError;
            ++nAccepted;
        }

        // Adjust step size
        auto acceptanceRate = double(nAccepted) / (iter + 1);
        stepSize_ *= nAccepted == 0 ? 0.8 : acceptanceRate / targetAcceptanceRatio_;

        // Run sampling function?
        if (samplingFunction_ && samplingFrequency_ > 0 && ((iter + 1) % samplingFrequency_ == 0))
            samplingFunction_(values);

        if (maxIterations_ < 10 || (iter % (maxIterations_ / 10) == 0))
            Messenger::printVerbose("MonteCarloMinimiser::minimise() - Iteration {} error = {}, stepSize = {}\n", iter + 1,
                                    currentError, stepSize_);
    }

    // Set optimised values back into their original variables
    pokeValues(values);

    return currentError;
}
