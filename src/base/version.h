// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

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
    VersionCounter &operator++();
};
