/*
 */

#pragma once

#include "base/messenger.h"
#include "math/minimiser.h"
#include <iomanip>

template <class T> class MonteCarloMinimiser : public MinimiserBase<T>
{
    public:
    MonteCarloMinimiser<T>(T &object, typename MinimiserBase<T>::MinimiserCostFunction costFunction,
                           bool pokeBeforeCost = false)
        : MinimiserBase<T>(object, costFunction, pokeBeforeCost)
    {
        parameterSmoothingFrequency_ = 0;
        acceptanceMemoryLength_ = 25;
        targetAcceptanceRatio_ = 0.33;
        maxIterations_ = 100;
        stepSize_ = 0.1;
        minStepSize_ = 1.0e-5;
    }

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

    private:
    // Smooth current parameter set
    void smoothParameters(Array<double> &values)
    {
        // Apply Kolmogorov–Zurbenko filter
        for (int k = 0; k < parameterSmoothingK_; ++k)
        {
            Array<double> newY(values.nItems());
            newY = 0.0;
            int n, m, i = parameterSmoothingM_ / 2;

            // Left-most region of data
            for (n = 0; n < i; ++n)
            {
                for (m = 0; m <= n + i; ++m)
                    newY[n] += values[m];
                newY[n] /= (i + 1 + n);
            }

            // Central region (full average width available)
            for (n = i; n < values.nItems() - i; ++n)
            {
                for (m = n - i; m <= n + i; ++m)
                    newY[n] += (values[m]);
                newY[n] /= parameterSmoothingM_;
            }

            // Right-most region of data
            for (n = values.nItems() - i; n < values.nItems(); ++n)
            {
                for (m = n - i; m < values.nItems(); ++m)
                    newY[n] += values[m];
                newY[n] /= (values.nItems() - n + i + 1);
            }

            for (int n = 0; n < values.nItems(); ++n)
                values[n] = newY[n];
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
    double execute(Array<double> &values)
    {
        // Get initial error of input parameters
        double currentError = MinimiserBase<T>::cost(values);
        Messenger::printVerbose("MonteCarloMinimiser<T>::minimise() - Initial error = {}\n", currentError);

        double trialError;
        Array<double> trialValues;
        Array<int> accepts;
        bool accepted;
        int smoothingNAccepted = 0;

        // Outer loop
        for (int iter = 0; iter < maxIterations_; ++iter)
        {
            // Copy current best alpha
            trialValues = values;

            // Perform a Monte Carlo move on a random parameter
            int i = trialValues.nItems() * DissolveMath::random();
            if (i >= trialValues.nItems())
                i = trialValues.nItems() - 1;

            if (fabs(trialValues[i]) < 1.0e-8)
                trialValues[i] += DissolveMath::randomPlusMinusOne() * 0.01 * stepSize_;
            else
                trialValues[i] += DissolveMath::randomPlusMinusOne() * trialValues[i] * stepSize_;

            // Get error for the new parameters, and store if improved
            trialError = MinimiserBase<T>::cost(trialValues);
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
            accepts.add(accepted);

            // If we have enough acceptance memory, adjust the step size
            if (accepts.nItems() == acceptanceMemoryLength_)
            {
                double ratio = targetAcceptanceRatio_ / (double(accepts.sum()) / acceptanceMemoryLength_);
                stepSize_ /= ratio;
                if (stepSize_ < minStepSize_)
                    stepSize_ = minStepSize_;

                accepts.clear();
            }

            // Perform periodic smoothing if requested
            if ((parameterSmoothingFrequency_ > 0) && (smoothingNAccepted == parameterSmoothingFrequency_))
            {
                smoothParameters(values);
                currentError = MinimiserBase<T>::cost(values);
                smoothingNAccepted = 0;
            }

            if (iter % (maxIterations_ / 10) == 0)
                Messenger::printVerbose("MonteCarloMinimiser<T>::minimise() - Iteration {} error = {}, stepSize = {}\n",
                                        iter + 1, currentError, stepSize_);
        }

        return currentError;
    }
};
