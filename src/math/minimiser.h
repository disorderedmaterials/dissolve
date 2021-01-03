// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "expression/variable.h"
#include <vector>

// Minimiser Base Class
template <class T> class MinimiserBase
{
    public:
    // Cost function pointer typedef
    typedef double (T::*MinimiserCostFunction)(const std::vector<double> &alpha);
    MinimiserBase<T>(T &object, MinimiserCostFunction costFunction, bool pokeBeforeCost = false)
        : object_(object), costFunction_(costFunction)
    {
        penaltyPower_ = 2;
        penaltyFactor_ = 1e5;
        pokeBeforeCost_ = pokeBeforeCost;
    }

    /*
     * Object Target
     */
    protected:
    // Object used to call cost function
    T &object_;

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

    public:
    // Set whether to poke values before assessing cost
    void setPokeBeforeCost(bool b) { pokeBeforeCost_ = b; }

    /*
     * Target Parameters
     */
    protected:
    // Pointers to double values to be fit
    std::vector<double *> targets_;
    // Whether maximum limits have been set for targets
    std::vector<bool> maximumLimit_;
    // Whether minimum limits have been set for targets
    std::vector<bool> minimumLimit_;
    // Maximum limiting values for targets
    std::vector<double> maximumValue_;
    // Minimum limiting values for targets
    std::vector<double> minimumValue_;
    // Integer power of penalty function when outside allowable limit
    int penaltyPower_;
    // Scaling factor for penalties incurred when outside of allowable limit
    double penaltyFactor_;

    private:
    void pokeValues(const std::vector<double> &values)
    {
        for (auto n = 0; n < targets_.size(); ++n)
            (*targets_[n]) = values[n];
    }

    public:
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
    // Add ExpressionVariable as target, with limits specified
    void addTarget(std::shared_ptr<ExpressionVariable> var, bool minLimit = false, double minValue = 0.0, bool maxLimit = false,
                   double maxValue = 0.0)
    {
        addTarget(var->valuePointer()->doublePointer(), minLimit, minValue, maxLimit, maxValue);
    }
    // Add array of pointers to targets
    void addTargets(std::vector<double *> vars, bool minLimit = false, double minValue = 0.0, bool maxLimit = false,
                    double maxValue = 0.0)
    {
        for (auto n = 0; n < vars.size(); ++n)
            addTarget(vars[n], minLimit, minValue, maxLimit, maxValue);
    }
    // Set penalty power for values outside of set limits
    void setPenaltyPower(int power) { penaltyPower_ = power; }
    // Set penalty factor for values outside of set limits
    void setPenaltyFactor(double factor) { penaltyFactor_ = factor; }

    /*
     * Minimise
     */
    protected:
    // Minimiser function to be called in derived class
    virtual double execute(std::vector<double> &values) = 0;

    public:
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
};
