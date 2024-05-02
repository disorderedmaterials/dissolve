// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/randomBuffer.h"

RandomBuffer::RandomBuffer(const ProcessPool &procPool, ProcessPool::CommunicatorType commType,
                           OptionalReferenceWrapper<Timer> commsTimer)
    : processPool_(procPool), index_(0), commsGroup_(commType), commsTimer_(commsTimer)
{
    refill();
}

RandomBuffer::RandomBuffer(const ProcessPool &procPool, ProcessPool::DivisionStrategy strategy,
                           OptionalReferenceWrapper<Timer> commsTimer)
    : processPool_(procPool), index_(0), commsTimer_(commsTimer)
{
    reset(strategy);
}

// Refill random number buffer
void RandomBuffer::refill()
{
#ifdef PARALLEL
    // Reset index
    index_ = 0;

    // Generate new random numbers in array
    if (commsGroup_ == ProcessPool::PoolProcessesCommunicator)
    {
        // Master creates buffer and sends to slaves
        if (processPool_.isMaster())
        {
            std::generate(buffer_.begin(), buffer_.end(), []() { return DissolveMath::random(); });
            processPool_.broadcast(buffer_.data(), RANDBUFFERSIZE, 0, commsGroup_);
        }
        else
            processPool_.broadcast(buffer_.data(), RANDBUFFERSIZE, 0, commsGroup_);
    }
    else if (commsGroup_ == ProcessPool::GroupLeadersCommunicator)
    {
        // Master creates buffer and sends to slaves
        if (processPool_.isMaster())
        {
            std::generate(buffer_.begin(), buffer_.end(), []() { return DissolveMath::random(); });
            processPool_.broadcast(buffer_.data(), RANDBUFFERSIZE, 0, commsGroup_);
        }
        else
            processPool_.broadcast(buffer_.data(), RANDBUFFERSIZE, 0, commsGroup_);
    }
    else if (commsGroup_ == ProcessPool::GroupProcessesCommunicator)
    {
        // Group leader creates buffer and sends to slaves
        if (processPool_.groupLeader())
        {
            std::generate(buffer_.begin(), buffer_.end(), []() { return DissolveMath::random(); });
            processPool_.broadcast(buffer_.data(), RANDBUFFERSIZE, 0, commsGroup_);
        }
        else
            processPool_.broadcast(buffer_.data(), RANDBUFFERSIZE, 0, commsGroup_);
    }
    else
    {
        // Create own random buffer
        std::generate(buffer_.begin(), buffer_.end(), []() { return DissolveMath::random(); });
    }
#endif
}

// Reset buffer
void RandomBuffer::reset(ProcessPool::CommunicatorType commType)
{
    commsGroup_ = commType;
    refill();
}
void RandomBuffer::reset(ProcessPool::DivisionStrategy strategy)
{
    switch (strategy)
    {
        case (ProcessPool::GroupsStrategy):
            commsGroup_ = ProcessPool::GroupLeadersCommunicator;
            break;
        case (ProcessPool::GroupProcessesStrategy):
            commsGroup_ = ProcessPool::GroupProcessesCommunicator;
            break;
        case (ProcessPool::PoolStrategy):
            commsGroup_ = ProcessPool::PoolProcessesCommunicator;
            break;
        case (ProcessPool::PoolProcessesStrategy):
            commsGroup_ = ProcessPool::NoCommunicator;
            break;
    }

    refill();
}

// Get next buffered random number
double RandomBuffer::random()
{
#ifdef PARALLEL
    // Have we exhausted the buffer?
    if (index_ == RANDBUFFERSIZE)
        refill();
    return buffer_[index_++];
#else
    return DissolveMath::random();
#endif
}

// Get next buffered random number (-1 to +1 inclusive)
double RandomBuffer::randomPlusMinusOne()
{
#ifdef PARALLEL
    // Have we exhausted the buffer?
    if (index_ == RANDBUFFERSIZE)
        refill();
    return (buffer_[index_++] - 0.5) * 2.0;
#else
    return DissolveMath::randomPlusMinusOne();
#endif
}
