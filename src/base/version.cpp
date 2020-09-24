// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "base/version.h"
#include "base/processpool.h"

VersionCounter::VersionCounter() { version_ = 0; }

VersionCounter::~VersionCounter() {}

/*
 * Version Information
 */

// Reset version counter to zero
void VersionCounter::zero() { version_ = 0; }

/*
 * Operators
 */

// Automatic conversion to integer
VersionCounter::operator int() const { return version_; }

// Prefix increment
int VersionCounter::operator++() { return (++version_); }

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool VersionCounter::broadcast(ProcessPool &procPool, const int root)
{
#ifdef PARALLEL
    if (!procPool.broadcast(version_, root))
        return false;
#endif
    return true;
}
