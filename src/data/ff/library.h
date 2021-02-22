// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include <memory>
#include <string>

// Forward Declarations
/* none */

// Forcefield Library
class ForcefieldLibrary
{
    private:
    // List of all available forcefields
    static std::vector<std::shared_ptr<Forcefield>> forcefields_;

    private:
    // Set up supplied forcefield for use, and add to internal list
    static bool registerForcefield(std::shared_ptr<Forcefield> ff);
    // Register Forcefields for use
    static void registerForcefields();

    public:
    // Return list of available Forcefields
    static std::vector<std::shared_ptr<Forcefield>> &forcefields();
    // Return named Forcefield, if it exists
    static std::shared_ptr<Forcefield> forcefield(std::string_view name);
    // Add a Forcefield from a file, if it exists
    static std::shared_ptr<Forcefield> forcefield(std::string_view name, std::string_view file);
};
