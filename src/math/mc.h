/*
 */

#pragma once

#include "base/messenger.h"
#include "expression/variable.h"
#include "math/mathfunc.h"
#include <iomanip>
#include <numeric>

template <class T> class MonteCarloMinimiser
{
    using MinimiserCostFunction = double (T::*)(const std::vector<double> &);

    public:
    MonteCarloMinimiser<T>(T &object, MinimiserCostFunction costFunction, bool pokeBeforeCost = false)
        : object_(object), costFunction_(costFunction), pokeBeforeCost_(pokeBeforeCost)
    {
        parameterSmoothingFrequency_ = 0;
        acceptanceMemoryLength_ = 25;
        targetAcceptanceRatio_ = 0.33;
        maxIterations_ = 100;
        stepSize_ = 0.1;
        minStepSize_ = 1.0e-5;
    }

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
    // Object used to call cost function
    T &object_;
    // Pointer to cost function
    MinimiserCostFunction costFunction_;
    // Pointers to double values to be fit
    std::vector<double *> targets_;
    // Whether maximum limits have been set for targets
    std::vector<bool> maximumLimit_;
    // Whether minimum limits have been set for targets
    std::vector<bool> minimumLimit_;
    // Scaling factor for penalties incurred when outside of allowable limit
    double penaltyFactor_;
    // Minimum limiting values for targets
    std::vector<double> minimumValue_;
    // Maximum limiting values for targets
    std::vector<double> maximumValue_;
    // Integer power of penalty function when outside allowable limit
    int penaltyPower_;

    private:
    void pokeValues(const std::vector<double> &values)
    {
        for (auto n = 0; n < targets_.size(); ++n)
            (*targets_[n]) = values[n];
    }
    // Calculate cost from specified values, including contributions from any supplied limits
    double cost(const std::vector<double> &alpha)
    {
        // Poke values into targets before calling cost function?
        if (pokeBeforeCost_)
            pokeValues(alpha);

        // Evaluate cost function
        double x = (object_.*costFunction_)(alpha);

        // Add penalties from values exceeding set limits
        for (auto n = 0; n < alpha.size(); ++n)
        {
            // Minimum limit
            if (minimumLimit_[n] && (alpha[n] < minimumValue_[n]))
                x += penaltyFactor_ * pow(minimumValue_[n] - alpha[n], penaltyPower_);

            // Minimum limit
            if (maximumLimit_[n] && (alpha[n] > maximumValue_[n]))
                x += penaltyFactor_ * pow(alpha[n] - maximumValue_[n], penaltyPower_);
        }

        return x;
    }
    // Smooth current parameter set
    void smoothParameters(std::vector<double> &values)
    {
        // Apply Kolmogorov–Zurbenko filter
        for (auto k = 0; k < parameterSmoothingK_; ++k)
        {
            std::vector<double> newY(values.size(), 0.0);
            int n, m, i = parameterSmoothingM_ / 2;

            // Left-most region of data
            for (n = 0; n < i; ++n)
            {
                for (m = 0; m <= n + i; ++m)
                    newY[n] += values[m];
                newY[n] /= (i + 1 + n);
            }

            // Central region (full average width available)
            for (n = i; n < values.size() - i; ++n)
            {
                for (m = n - i; m <= n + i; ++m)
                    newY[n] += (values[m]);
                newY[n] /= parameterSmoothingM_;
            }

            // Right-most region of data
            for (n = values.size() - i; n < values.size(); ++n)
            {
                for (m = n - i; m < values.size(); ++m)
                    newY[n] += values[m];
                newY[n] /= (values.size() - n + i + 1);
            }

            std::copy(newY.begin(), newY.end(), values.begin());
        }
    }

    public:
    // Set maximum number of iterations to perform
    void setMaxIterations(int maxIterations) { maxIterations_ = maxIterations; }
    // Set step size
    void setStepSize(double stepSize) { stepSize_ = stepSize; }
    // Return step size
    double stepSize() { return stepSize_; }
    // Set minimum step size
    void setMinStepSize(double minStepSize) { minStepSize_ = minStepSize; }
    // Enable parameter smoothing
    void enableParameterSmoothing(int everyNAccepted, int smoothK, int smoothM)
    {
        parameterSmoothingFrequency_ = everyNAccepted;
        parameterSmoothingK_ = smoothK;
        parameterSmoothingM_ = smoothM;

        // Make sure 'm' is odd
        if (parameterSmoothingM_ % 2 == 0)
            --parameterSmoothingM_;
    }
    // Set acceptance memory length
    void setAcceptanceMemoryLength(int length) { acceptanceMemoryLength_ = length; }
    // Target acceptance ratio
    void setTargetAcceptanceRatio(double ratio) { targetAcceptanceRatio_ = ratio; }
    // Perform minimisation
    double execute(std::vector<double> &values)
    {
        // Get initial error of input parameters
        double currentError = cost(values);
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

            // Perform periodic smoothing if requested
            if ((parameterSmoothingFrequency_ > 0) && (smoothingNAccepted == parameterSmoothingFrequency_))
            {
                smoothParameters(values);
                currentError = cost(values);
                smoothingNAccepted = 0;
            }

            if (maxIterations_ < 10 || (iter % (maxIterations_ / 10) == 0))
                Messenger::printVerbose("MonteCarloMinimiser<T>::minimise() - Iteration {} error = {}, stepSize = {}\n",
                                        iter + 1, currentError, stepSize_);
        }

        return currentError;
    }
    // Minimise target parameters
    double minimise()
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

        // Minimise the function
        double finalCost = execute(values);

        // Set optimised values back into their original variables
        pokeValues(values);

        return finalCost;
    }
    // Add pointer as fit target, with limits specified
    void addTarget(double *var, bool minLimit = false, double minValue = 0.0, bool maxLimit = false, double maxValue = 0.0)
    {
        // Add pointer and current value
        targets_.push_back(var);

        // Add/set limits
        minimumLimit_.push_back(minLimit);
        minimumValue_.push_back(minValue);
        maximumLimit_.push_back(maxLimit);
        maximumValue_.push_back(maxValue);
    }
    // Add reference as fit target, with limits specified
    void addTarget(double &var, bool minLimit = false, double minValue = 0.0, bool maxLimit = false, double maxValue = 0.0)
    {
        addTarget(&var, minLimit, minValue, maxLimit, maxValue);
    }
    void addTarget(const std::shared_ptr<ExpressionVariable> &var, bool minLimit = false, double minValue = 0.0,
                   bool maxLimit = false, double maxValue = 0.0)
    {
        addTarget(var->valuePointer()->doublePointer(), minLimit, minValue, maxLimit, maxValue);
    }
};
