// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#include "parallel_defs.h"

namespace dissolve
{
// A combinable container used in multithreading operations
// The local method retrives a thread local object during the multithreading computations
// We then use finalize to combine each thread local storage instance into the parent container
// using std::transform

// Usage:
// - Create an instance by passing in the parent container and a lambda initializer which specifies how to create local
// instances of the container
// - Capture the combinable by reference in the lambda operator of the parallel operation
// - Within the lambda operator call local() to access a thread local version of the container
// - After the parallel operation call finaliize to accumulate into the parent container.
template <class Container> class CombinableContainer
{
    public:
    template <typename Lambda>
    CombinableContainer(Container &parent, Lambda initializer) : parent_(parent), combinable_(initializer)
    {
    }
    void finalize()
    {
        auto combineOp = [this](const auto &localContainer) {
            std::transform(parent_.begin(), parent_.end(), localContainer.begin(), parent_.begin(), std::plus<>{});
        };
        combinable_.combine_each(combineOp);
    }

    Container &local() { return combinable_.local(); }

    private:
    Container &parent_;
    dissolve::combinable<Container> combinable_;
};
} // namespace dissolve