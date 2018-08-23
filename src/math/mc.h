/*
 */

#ifndef DISSOLVE_MCMIN_H
#define DISSOLVE_MCMIN_H

#include "base/messenger.h"
#include <iomanip>

template <class T> class MonteCarloMinimiser
{
	// Command pointer typedef
	typedef double (T::*MonteCarloMinimiserCostFunction)(const Array<double>& alpha);

	public:
	// Constructor
	MonteCarloMinimiser<T>(T& object, MonteCarloMinimiserCostFunction func) : object_(object), costFunction_(func)
	{
		parameterSmoothingFrequency_ = 0;
		acceptanceMemoryLength_ = 25;
		targetAcceptanceRatio_ = 0.33;
	}

	private:
	// Object used to call specified function
	T& object_;
	// Pointer to cost function
	MonteCarloMinimiserCostFunction costFunction_;
	// Pointers to double values to be fit
	Array<double*> targets_;
	// Parameter smoothing frequency (required number of accepted moves)
	int parameterSmoothingFrequency_;
	// Parameter smoothing control (Kolmogorov-Zurbenko)
	int parameterSmoothingK_, parameterSmoothingM_;
	// Step size
	double stepSize_;
	// Acceptance memory length
	int acceptanceMemoryLength_;
	// Target acceptance ratio
	double targetAcceptanceRatio_;

	private:
	// Return Array of fit target values
	Array<double> currentTargetValues()
	{
		Array<double> values;
		for (int n=0; n<targets_.nItems(); ++n) values.add(*targets_.constAt(n));
		return values;
	}
	// Set Array of fit target values
	void setCurrentTargetValues(Array<double> values)
	{
		for (int n=0; n<targets_.nItems(); ++n) (*targets_.at(n)) = values.constAt(n);
	}
	// Smooth current parameter set
	void smoothParameters()
	{
		// Apply Kolmogorov–Zurbenko filter
		for (int k = 0; k<parameterSmoothingK_; ++k)
		{
			Array<double> newY(targets_.nItems());
			newY = 0.0;
			int n, m, i = parameterSmoothingM_/2;

			// Left-most region of data
			for (n=0; n<i; ++n)
			{
				for (m=0; m<=n+i; ++m) newY[n] += (*targets_[m]);
				newY[n] /= (i + 1 + n);
			}

			// Central region (full average width available)
			for (n=i; n < targets_.nItems()-i; ++n)
			{
				for (m=n-i; m <= n+i; ++m) newY[n] += (*targets_[m]);
				newY[n] /= parameterSmoothingM_;
			}

			// Right-most region of data
			for (n=targets_.nItems()-i; n<targets_.nItems(); ++n)
			{
				for (m=n-i; m<targets_.nItems(); ++m) newY[n] += (*targets_[m]);
				newY[n] /= (targets_.nItems() - n + i + 1);
			}

			for (int n=0; n<targets_.nItems(); ++n) (*targets_[n]) = newY[n];
		}
	}

	public:
	// Add double value to be fit
	void addTarget(double& var)
	{
		targets_.add(&var);
	}
	// Add Array of double values to be fit
	void addTargets(Array<double> variables)
	{
		for (int n=0; n<variables.nItems(); ++n) targets_.add(&(variables.array()[n]));
	}
	// Enable parameter smoothing
	void enableParameterSmoothing(int everyNAccepted, int smoothK, int smoothM)
	{
		parameterSmoothingFrequency_ = everyNAccepted;
		parameterSmoothingK_ = smoothK;
		parameterSmoothingM_ = smoothM;

		// Make sure 'm' is odd
		if (parameterSmoothingM_%2 == 0) --parameterSmoothingM_;
	}
	// Set acceptance memory length
	void setAcceptanceMemoryLength(int length)
	{
		acceptanceMemoryLength_ = length;
	}
	// Target acceptance ratio
	void setTargetAcceptanceRatio(double ratio)
	{
		targetAcceptanceRatio_ = ratio;
	}

	// Perform minimisation
	double minimise(const int maxIterations, double stepSize, const double stepSizeMin = 1.0e-5)
	{
		// Get initial error of input parameters
		double currentError = (object_.*costFunction_)(currentTargetValues());
		Messenger::printVerbose("MonteCarloMinimiser<T>::minimise() - Initial error = %f\n", currentError);
		
		double trialError;
		stepSize_ = stepSize;
		Array<double> trialValues;
		Array<int> accepts;
		bool accepted;
		int smoothingNAccepted = 0;

		// Outer loop
		for (int iter=0; iter<maxIterations; ++iter)
		{
			// Copy current best alpha
			trialValues = currentTargetValues();

			// Perform a Monte Carlo move on the parameters
			for (int i=0; i<trialValues.nItems(); ++i) trialValues[i] += DissolveMath::randomPlusMinusOne()*stepSize_;

			// Get new error for the large move
			trialError = (object_.*costFunction_)(trialValues);

			// If we accept this large move, perform a fine adjustment around the new parameters
			accepted = trialError < currentError;
			if (accepted)
			{
				setCurrentTargetValues(trialValues);
				currentError = trialError;
				++smoothingNAccepted;
			}

			// Accumulate acceptance memory
			accepts.add(accepted);

			// If we have enough acceptance memory, adjust the step size
			if (accepts.nItems() == acceptanceMemoryLength_)
			{
				double ratio = targetAcceptanceRatio_ / (double(accepts.sum()) / acceptanceMemoryLength_);
				stepSize_ /= ratio;
				if (stepSize_ < stepSizeMin) stepSize_ = stepSizeMin;

				accepts.clear();
			}

			// Perform periodic smoothing if requested
			if ((parameterSmoothingFrequency_ > 0) && (smoothingNAccepted == parameterSmoothingFrequency_))
			{
				smoothParameters();
				currentError = (object_.*costFunction_)(currentTargetValues());
				smoothingNAccepted = 0;
			}

			if (iter%100 == 0) Messenger::printVerbose("MonteCarloMinimiser<T>::minimise() - Iteration %04i error = %f, stepSize = %f\n", iter+1, currentError, stepSize_);
		}

		return currentError;
	}
	// Return step size
	double stepSize()
	{
		return stepSize_;
	}
};

#endif
