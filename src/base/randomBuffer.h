// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

// Random number buffer size
#define RANDBUFFERSIZE 16172

#include "base/processPool.h"
#include "templates/optionalRef.h"
#include <array>
// Include <mpi.h> only if we are compiling in parallel
#ifdef PARALLEL
#include <mpi.h>
#endif

// RandomBuffer
class RandomBuffer
{
    public:
    RandomBuffer(const ProcessPool &procPool, ProcessPool::CommunicatorType commType,
                 OptionalReferenceWrapper<Timer> commsTimer = {});
    RandomBuffer(const ProcessPool &procPool, ProcessPool::DivisionStrategy strategy,
                 OptionalReferenceWrapper<Timer> commsTimer = {});

    private:
    // Process pool in which the random buffer is operating
    const ProcessPool &processPool_;
    // Random number buffer
    std::array<double, RANDBUFFERSIZE> buffer_;
    // Index of next buffered number
    std::size_t index_;
    // Communicator for random number buffer
    ProcessPool::CommunicatorType commsGroup_;
    // Optional timer for capturing communication time
    OptionalReferenceWrapper<Timer> commsTimer_;

    private:
    // Refill random number buffer
    void refill();

    public:
    // Reset random buffer
    void reset(ProcessPool::CommunicatorType commType);
    void reset(ProcessPool::DivisionStrategy strategy);
    // Get next buffered random number (0-1 inclusive)
    double random();
    // Get next buffered random number (-1 to +1 inclusive)
    double randomPlusMinusOne();
};
