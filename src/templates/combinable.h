// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "templates/parallelDefs.h"

namespace dissolve
{
// A combinable container used in multithreading operations
// The local method retrieves a thread local object during the multithreading computations
// We then use finalize to combine each thread local storage instance into the parent container
// using std::transform

// Usage:
// - Create an instance by passing in the parent container and a lambda initializer which specifies how to create local
// instances of the container
// - Capture the combinable by reference in the lambda operator of the parallel operation
// - Within the lambda operator call local() to access a thread local version of the container
// - After the parallel operation call finalize to accumulate into the parent container.
template <class Container> class CombinableContainer
{
    public:
    template <typename Lambda>
    CombinableContainer(Container &parent, Lambda initializer) : parent_(parent), combinable_(initializer)
    {
    }
    void finalize()
    {
        auto combineOp = [this](const auto &localContainer)
        { std::transform(parent_.begin(), parent_.end(), localContainer.begin(), parent_.begin(), std::plus<>{}); };
        combinable_.combine_each(combineOp);
    }

    Container &local() { return combinable_.local(); }

    private:
    Container &parent_;
    dissolve::combinable<Container> combinable_;
};

// A combinable value used in multithreading operations
// The local method retrieves a thread local object during the multithreading computations
// We then use finalize to combine each thread local storage instance into a final value
// using the std::plus operator

// Usage:
// - Create an instance by passing in a lambda initializer which specifies how to create local
// instances of the value type
// - Capture the combinable by reference in the lambda operator of the parallel operation
// - Within the lambda operator call local() to access a thread local version of the container
// - After the parallel operation call finalize to retrieve the final accumulated value.
template <class ValueType> class CombinableValue
{
    public:
    template <typename Lambda> CombinableValue(Lambda initializer) : combinable_(initializer) {}
    CombinableValue(ValueType initial) : combinable_([initial]() { return initial; }) {}
    ValueType finalize() { return combinable_.combine(std::plus<ValueType>()); }
    ValueType &local() { return combinable_.local(); }

    private:
    dissolve::combinable<ValueType> combinable_;
};

// A combinable functor used in multithreading operations
// The local method retrieves a thread local object during the multithreading computations

// Usage:
// - Create an instance by passing in a lambda initializer which specifies how to create local
// instances of the value type
// - Capture the combinable by reference in the lambda operator of the parallel operation
// - Within the lambda operator call local() to access a thread local version of the container
template <class ClassType> class CombinableFunctor
{

    public:
    template <typename Lambda> CombinableFunctor(Lambda initializer) : combinable_(initializer) {}
    ClassType &local() { return combinable_.local(); }

    private:
    dissolve::combinable<ClassType> combinable_;
};

} // namespace dissolve
