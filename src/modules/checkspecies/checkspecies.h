/*
    *** CheckSpecies Module
    *** src/modules/checkspecies/checkspecies.h
    Copyright T. Youngs 2012-2020

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

#include "module/module.h"
#include <tuple>
#include <vector>

// Forward Declarations
/* none */

// CheckSpecies Module
class CheckSpeciesModule : public Module
{
    public:
    CheckSpeciesModule();
    ~CheckSpeciesModule();

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const;
    // Return category for module
    std::string_view category() const;
    // Return brief description of module
    std::string_view brief() const;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const;

    /*
     * Initialisation
     */
    protected:
    // Perform any necessary initialisation for the Module
    void initialise();

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool);

    /*
     * Functions / Data
     */
    private:
    // Check supplied parameter vectors for consistency, returning false if any differ by the supplied tolerance
    bool checkParameters(const std::vector<double> &source, const std::vector<double> &ref, const double tolerance);

    private:
    // Atom types to check against indices
    std::vector<std::tuple<std::vector<int>, std::vector<std::string>>> atomTypes_;
    // Bond parameters to check
    std::vector<std::tuple<std::vector<int>, std::vector<double>>> bondParameters_;
};
