/*
    *** Calculate Average Molecule Module
    *** src/modules/calculate_avgmol/calculate_avgmol.h
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

#include "classes/species.h"
#include "module/module.h"

// Forward Declarations
/* none */

// Calculate Average Molecule Module
class CalculateAvgMolModule : public Module
{
    public:
    CalculateAvgMolModule();
    ~CalculateAvgMolModule();

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
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool);
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool);

    /*
     * Functions / Data
     */
    private:
    // Species targeted by module (derived from selected site)
    Species *targetSpecies_;
    // Local Species representing average of targeted Species
    Species averageSpecies_;

    private:
    // Ensure arrays are the correct size for the current target Species
    void updateArrays(Dissolve &dissolve);
    // Update the local species with the coordinates from the supplied arrays
    void updateSpecies(const Array<SampledDouble> &x, const Array<SampledDouble> &y, const Array<SampledDouble> &z);

    public:
    // Return average Species
    Species &averageSpecies();

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
