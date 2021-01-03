// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

// Forward Declarations
class CoreData;
class ProcessPool;

// Version Counter
class VersionCounter
{
    public:
    VersionCounter();
    ~VersionCounter();

    /*
     * Version Information
     */
    private:
    // Version counter
    int version_;

    public:
    // Reset version counter to zero
    void zero();

    /*
     * Operators
     */
    public:
    // Automatic conversion to integer
    operator int() const;
    // Prefix increment
    int operator++();

    /*
     * Parallel Comms
     */
    public:
    // Broadcast data from Master to all Slaves
    bool broadcast(ProcessPool &procPool, const int root);
};
