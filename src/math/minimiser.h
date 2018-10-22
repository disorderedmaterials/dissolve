/*
	*** Minimiser Base
	*** src/math/minimiser.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_MINIMISER_H
#define DISSOLVE_MINIMISER_H

#include "base/messenger.h"
#include "templates/array.h"

// Minimiser Base Class
template <class T> class MinimiserBase
{
	public:
	// Cost function pointer typedef
	typedef double (T::*MinimiserCostFunction)(const Array<double>& alpha);
	// Constructor
	MinimiserBase<T>(T& object, MinimiserCostFunction costFunction) : object_(object), costFunction_(costFunction)
	{
		penaltyPower_ = 2;
		penaltyFactor_ = 1e5;
		pokeBeforeCost_ = false;
	}


	/*
	 * Object Target
	 */
	protected:
	// Object used to call cost function
	T& object_;


	/*
	 * Cost Function
	 */
	private:
	// Whether to poke values into targets before calling the cost function
	bool pokeBeforeCost_;

	protected:
	// Pointer to cost function
	MinimiserCostFunction costFunction_;

	protected:
	// Calculate cost from specified values, including contributions from any supplied limits
	double cost(const Array<double>& alpha)
	{
		// Poke values into targets before calling cost function?
		if (pokeBeforeCost_) pokeValues(alpha);

		// Evaluate cost function
		double x = (object_.*costFunction_)(alpha);

		// Add penalties from values exceeding set limits
		for (int n=0; n<alpha.nItems(); ++n)
		{
			// Minimum limit
			if (minimumLimit_[n] && (alpha.constAt(n) < minimumValue_[n])) x += penaltyFactor_ * pow(minimumValue_[n] - alpha.constAt(n), penaltyPower_);

			// Minimum limit
			if (maximumLimit_[n] && (alpha.constAt(n) > maximumValue_[n])) x += penaltyFactor_ * pow(alpha.constAt(n) - maximumValue_[n], penaltyPower_);
		}

		return x;
	}

	public:
	// Set whether to poke values before assessing cost
	void setPokeBeforeCost(bool b)
	{
		pokeBeforeCost_ = b;
	}


	/*
	 * Target Parameters
	 */
	protected:
	// Pointers to double values to be fit
	Array<double*> targets_;
	// Local values that are the subject of fitting
	Array<double> values_;
	// Whether maximum limits have been set for targets
	Array<bool> maximumLimit_;
	// Whether minimum limits have been set for targets
	Array<bool> minimumLimit_;
	// Maximum limiting values for targets
	Array<double> maximumValue_;
	// Minimum limiting values for targets
	Array<double> minimumValue_;
	// Integer power of penalty function when outside allowable limit
	int penaltyPower_;
	// Scaling factor for penalties incurred when outside of allowable limit
	double penaltyFactor_;

	private:
	void pokeValues(const Array<double>& values)
	{
		for (int n=0; n<targets_.nItems(); ++n) (*targets_[n]) = values.constAt(n);
	}

	public:
	// Add pointer as fit target, with limits specified
	void addTarget(double* var, bool minLimit = false, double minValue = 0.0, bool maxLimit = false, double maxValue = 0.0)
	{
		// Add pointer and current value
		targets_.add(var);
		values_.add(*var);

		// Add/set limits
		minimumLimit_.add(minLimit);
		minimumValue_.add(minValue);
		maximumLimit_.add(maxLimit);
		maximumValue_.add(maxValue);
	}
	// Add reference as fit target, with limits specified
	void addTarget(double& var, bool minLimit = false, double minValue = 0.0, bool maxLimit = false, double maxValue = 0.0)
	{
		addTarget(&var, minLimit, minValue, maxLimit, maxValue);
	}
	// Add array of pointers to targets
	void addTargets(Array<double*> vars, bool minLimit = false, double minValue = 0.0, bool maxLimit = false, double maxValue = 0.0)
	{
		for (int n=0; n<vars.nItems(); ++n) addTarget(vars[n], minLimit, minValue, maxLimit, maxValue);
	}
	// Set penalty power for values outside of set limits
	void setPenaltyPower(int power)
	{
		penaltyPower_ = power;
	}
	// Set penalty factor for values outside of set limits
	void setPenaltyFactor(double factor)
	{
		penaltyFactor_ = factor;
	}


	/*
	 * Minimise
	 */
	protected:
	// Minimiser function to be called in derived class
	virtual double execute(Array<double>& values, double tolerance) = 0;

	public:
	// Minimise target parameters
	double minimise(double tolerance = 1.0e-3)
	{
		// Check for zero variable parameters
		if (values_.nItems() == 0)
		{
			Messenger::warn("No variables specified for fitting, so nothing to do.\n");
			return 0.0;
		}

		// Minimise the function
		double value = execute(values_, tolerance);

		// Set minimised values back into their original variables
		pokeValues(values_);

		return value;
	}
};

#endif
