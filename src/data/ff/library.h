// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include <memory>
#include <string>

// Forcefield Library
class ForcefieldLibrary
{
    private:
    // List of all available forcefields
    static std::vector<std::shared_ptr<Forcefield>> forcefields_;

    private:
    // Register Forcefields for use
    static void registerForcefields();
    // Set up supplied forcefield for use, and add to internal list with no safety checks
    static bool registerForcefield_(const std::shared_ptr<Forcefield> &ff);

    public:
    // Set up supplied forcefield for use, and add to internal list
    static bool registerForcefield(std::shared_ptr<Forcefield> ff);
    // Return list of available Forcefields
    static std::vector<std::shared_ptr<Forcefield>> &forcefields();
    // Return named Forcefield, if it exists
    static std::shared_ptr<Forcefield> forcefield(std::string_view name);
};
