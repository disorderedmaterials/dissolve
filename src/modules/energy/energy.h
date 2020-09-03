/*
    *** Energy Module
    *** src/modules/energy/energy.h
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
class PotentialMap;

// Energy Module
class EnergyModule : public Module
{
    /*
     * Calculates the total energy of the system by one of several methods
     */

    public:
    EnergyModule();
    ~EnergyModule();

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

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool);

    /*
     * Functions
     */
    public:
    // Energy Stability Enum
    enum EnergyStability
    {
        NotAssessable = -1,
        EnergyStable = 0,
        EnergyUnstable = 1
    };
    // Return total interatomic energy of Configuration
    static double interAtomicEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap);
    // Return total interatomic energy of Species
    static double interAtomicEnergy(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap);
    // Return total intermolecular energy
    static double interMolecularEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap);
    // Return total intramolecular energy of Configuration
    static double intraMolecularEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap);
    // Return total intramolecular energy of Configuration, storing components in provided variables
    static double intraMolecularEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                       double &bondEnergy, double &angleEnergy, double &torsionEnergy, double &improperEnergy);
    // Return total intramolecular energy of Species
    static double intraMolecularEnergy(ProcessPool &procPool, Species *sp);
    // Return total energy (interatomic and intramolecular)
    static double totalEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap);
    // Return total energy (interatomic and intramolecular) of Configuration, storing components in provided variables
    static double totalEnergy(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap, double &interEnergy,
                              double &bondEnergy, double &angleEnergy, double &torsionEnergy, double &improperEnergy);
    // Return total energy (interatomic and intramolecular) of Species
    static double totalEnergy(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap);
    // Check energy stability of specified Configuration
    static EnergyStability checkStability(Configuration *cfg);
    // Check energy stability of specified Configurations, returning the number that are unstable
    static int nUnstable(const RefList<Configuration> &configurations);

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
