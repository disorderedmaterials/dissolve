/*
 */

#ifndef DISSOLVE_SD_H
#define DISSOLVE_SD_H

#include "base/messenger.h"
#include "templates/array.h"

template <class T> class SteepestDescent
{
	// Command pointer typedef
	typedef double (T::*SteepestDescentCostFunction)(double* alpha, int nAlpha);

	public:
	// Constructor
	SteepestDescent<T>(T& object, SteepestDescentCostFunction func) : object_(object), costFunction_(func)
	{
	}

	private:
	// Object used to call specified function
	T& object_;
	// Pointer to cost function
	SteepestDescentCostFunction costFunction_;
	// Pointers to double values to be fit
	Array<double*> targets_;
	// Local values for fitting
	Array<double> values_;


	private:
	// Calculate gradient of current parameters
	Array<double> gradient(const Array<double>& alpha)
	{
		const double deltaFrac = 0.05;
		double delta;
		Array<double> gradient;
		for (int n=0; n<alpha.nItems(); ++n)
		{
			Array<double> tempAlpha = alpha;
			tempAlpha[n] = alpha.value(n)*(1.0-deltaFrac);
			delta = (object_.*costFunction_)(tempAlpha.array(), tempAlpha.nItems());
			tempAlpha[n] = alpha.value(n)*(1.0+deltaFrac);
			delta -= (object_.*costFunction_)(tempAlpha.array(), tempAlpha.nItems());
			gradient.add(-delta / (2.0*deltaFrac));
		}

		return gradient;
	}
	// Adjust parameters along specified gradient
	Array<double> gradientMove(const Array<double>& alpha, const Array<double> grad, double stepSize)
	{
		Array<double> newAlpha(alpha.nItems());
		for (int n=0; n<alpha.nItems(); ++n) newAlpha[n] = alpha.value(n) * grad.value(n) * stepSize;
		return newAlpha;
	}

	public:
        // Add pointer to double value to be fit
        void addTarget(double& var)
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

		printf("Starting error = %f\n", currentError);
		for (int n=0; n<grad.nItems(); ++n) printf("GRAD %i = %f\n", n, grad[n]);
		for (cycle=0; cycle<maxIterations; cycle++)
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
				printf("Trial Alpha[10] = %f, step = %f, error = %f\n", trialAlpha[10], stepSize, trialError);
				if (trialError > currentError) stepSize *= 0.5;
			} while (trialError > currentError);
			
			printf("Current error = %f\n", trialError);
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

			if (lineDone || converged) break;
		}

		if (converged) Messenger::print("Steepest descent converged in %i steps.",cycle+1);
		else Messenger::print("Steepest descent did not converge within %i steps.",maxIterations);

		// Set minimised values back into their original variables
                for (int n=0; n<targets_.nItems(); ++n) (*targets_[n]) = values_[n];

		return currentError;
	}
};

#endif
