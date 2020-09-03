/*
    *** Steepest-Descent Minmiser
    *** src/math/sd.h
    Copyright T. Youngs 2019-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
        for (int n = 0; n < alpha.nItems(); ++n)
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
        for (int n = 0; n < alpha.nItems(); ++n)
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

            // Complex method begins here
            // 			else currentError = lineMinimise(sourceModel);

            // Copy alpha and calculate gradient ready for next cycle
            values_ = trialAlpha;
            grad = gradient(values_);

            // Check convergence criteria
            // 			if (fabs(deltaEnergy) < eConverge)
            // 			{
            // 				converged = true;
            // 				break;
            // 			}

            if (lineDone || converged)
                break;
        }

        if (converged)
            Messenger::print("Steepest descent converged in {} steps.", cycle + 1);
        else
            Messenger::print("Steepest descent did not converge within {} steps.", maxIterations);

        // Set minimised values back into their original variables
        for (int n = 0; n < targets_.nItems(); ++n)
            (*targets_[n]) = values_[n];

        return currentError;
    }
};
