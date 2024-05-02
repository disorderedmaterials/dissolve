// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

// Forward Declarations
class CoreData;

// Version Counter
class VersionCounter
{
    public:
    VersionCounter();
    ~VersionCounter() = default;

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
};
