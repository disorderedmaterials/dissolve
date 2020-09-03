/*
    *** Molecular Dynamics Module
    *** src/modules/md/md.h
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

// Forward Declarations
class Species;

// Molecular Dynamics Module
class MDModule : public Module
{
    /*
     * Module for testing various functions
     */

    public:
    MDModule();
    ~MDModule();

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
    private:
    // Species types to restrict calculation to (if any)
    RefList<Species> restrictToSpecies_;

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
     * Functions
     */
    private:
    // Cap forces in Configuration
    int capForces(Configuration *cfg, double maxForceSq, Array<double> &fx, Array<double> &fy, Array<double> &fz);
    // Determine timestep based on maximal force component
    double determineTimeStep(const Array<double> &fx, const Array<double> &fy, const Array<double> &fz);
};
