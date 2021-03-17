// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "math/minimiser.h"
#include "templates/array.h"

template <class T> class SteepestDescentMinimiser : public MinimiserBase<T>
{
    public:
    SteepestDescentMinimiser<T>(T &object, typename MinimiserBase<T>::MinimiserCostFunction costFunction,
                                bool pokeBeforeCost = false)
        : MinimiserBase<T>(object, costFunction, pokeBeforeCost)

              private :
        // Pointers to double values to be fit
        Array<double *> targets_;
    // Local values for fitting
    Array<double> values_;

    private:
    // Calculate gradient of current parameters
    Array<double> gradient(const Array<double> &alpha)
    {
        const double deltaFrac = 0.05;
        double delta;
        Array<double> gradient;
        for (auto n = 0; n < alpha.nItems(); ++n)
        {
            Array<double> tempAlpha = alpha;
            tempAlpha[n] = alpha.value(n) * (1.0 - deltaFrac);
            delta = (object_.*costFunction_)(tempAlpha.array(), tempAlpha.nItems());
            tempAlpha[n] = alpha.value(n) * (1.0 + deltaFrac);
            delta -= (object_.*costFunction_)(tempAlpha.array(), tempAlpha.nItems());
            gradient.add(-delta / (2.0 * deltaFrac));
        }

        return gradient;
    }
    // Adjust parameters along specified gradient
    Array<double> gradientMove(const Array<double> &alpha, const Array<double> grad, double stepSize)
    {
        Array<double> newAlpha(alpha.nItems());
        for (auto n = 0; n < alpha.nItems(); ++n)
            newAlpha[n] = alpha.value(n) * grad.value(n) * stepSize;
        return newAlpha;
    }

    public:
    // Add pointer to double value to be fit
    void addTarget(double &var)
    {
        targets_.add(&var);
        values_.add(var);
    }
    // Perform minimisation
    double minimise(int maxIterations)
    {
        int cycle, nattempts;
        double trialError, stepSize;
        bool lineDone, converged, success;
        Array<double> trialAlpha;

        /*
         * Prepare the calculation
         */

        converged = false;
        lineDone = false;

        // Calculate initial gradient and error
        double currentError = (object_.*costFunction_)(values_.array(), values_.nItems());
        Array<double> grad = gradient(values_);
        stepSize = 1.0;

        for (cycle = 0; cycle < maxIterations; cycle++)
        {
            // Minimise along gradient vector

            // Normalise the gradient
            grad /= grad.maxAbs();

            // Simple method begins here
            // Step along gradient (with reducing step size until energy decreases)
            stepSize = 0.1;
            do
            {
                trialAlpha = gradientMove(values_, grad, stepSize);
                trialError = (object_.*costFunction_)(trialAlpha.array(), trialAlpha.nItems());
                if (trialError > currentError)
                    stepSize *= 0.5;
            } while (trialError > currentError);

            currentError = trialError;

            // Copy alpha and calculate gradient ready for next cycle
            values_ = trialAlpha;
            grad = gradient(values_);

            if (lineDone || converged)
                break;
        }

        if (converged)
            Messenger::print("Steepest descent converged in {} steps.", cycle + 1);
        else
            Messenger::print("Steepest descent did not converge within {} steps.", maxIterations);

        // Set minimised values back into their original variables
        for (auto n = 0; n < targets_.nItems(); ++n)
            (*targets_[n]) = values_[n];

        return currentError;
    }
};
