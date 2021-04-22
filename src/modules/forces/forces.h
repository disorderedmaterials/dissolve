// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/export/forces.h"
#include "io/import/forces.h"
#include "module/module.h"
#include <memory>

// Forward Declarations
class Molecule;
class PotentialMap;

// Forces Module
class ForcesModule : public Module
{
    /*
     * Calculates the total forces in a system
     */

    public:
    ForcesModule();
    ~ForcesModule() override = default;

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const override;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const override;
    // Return category for module
    std::string_view category() const override;
    // Return brief description of module
    std::string_view brief() const override;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const override;

    /*
     * Initialisation
     */
    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

    /*
     * Data
     */
    private:
    // Reference forces for test
    ForceImportFileFormat referenceForces_;
    // Force export file and format
    ForceExportFileFormat exportedForces_;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * Functions
     */
    public:
    // Calculate interatomic forces within the specified Configuration
    static void interAtomicForces(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                  std::vector<Vec3<double>> &f);
    // Calculate interatomic forces on specified atoms within the specified Configuration
    static void interAtomicForces(ProcessPool &procPool, Configuration *cfg, const Array<int> &targetIndices,
                                  const PotentialMap &potentialMap, std::vector<Vec3<double>> &f);
    // Calculate interatomic forces within the specified Species
    static void interAtomicForces(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap,
                                  std::vector<Vec3<double>> &f);
    // Calculate total intramolecular forces acting on specific atoms in the Configuration
    static void intraMolecularForces(ProcessPool &procPool, Configuration *cfg, const Array<int> &targetIndices,
                                     const PotentialMap &potentialMap, std::vector<Vec3<double>> &f);
    // Calculate total intramolecular forces in Configuration
    static void intraMolecularForces(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                     std::vector<Vec3<double>> &f);
    // Calculate total intramolecular forces in Species
    static void intraMolecularForces(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap,
                                     std::vector<Vec3<double>> &f);
    // Calculate total forces within the specified Configuration
    static void totalForces(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                            std::vector<Vec3<double>> &f);
    // Calculate forces acting on specific atoms within the specified Configuration (arising from all atoms)
    static void totalForces(ProcessPool &procPool, Configuration *cfg, const Array<int> &targetIndices,
                            const PotentialMap &potentialMap, std::vector<Vec3<double>> &f);
    // Calculate forces acting on specific Molecules within the specified Configuration (arising from all atoms)
    static void totalForces(ProcessPool &procPool, Configuration *cfg, const Array<std::shared_ptr<Molecule>> &targetMolecules,
                            const PotentialMap &potentialMap, std::vector<Vec3<double>> &f);
    // Calculate total forces within the specified Species
    static void totalForces(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap, std::vector<Vec3<double>> &f);
};
