/*
    *** Forcefield Library
    *** src/data/fflibrary.h
    Copyright T. Youngs 2019-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "data/ff.h"
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
};
