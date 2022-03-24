// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
    public:
    ForcesModule();
    ~ForcesModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configurations
    Configuration *targetConfiguration_{nullptr};
    // Test parallel force routines against basic serial versions and supplied reference values (if provided)
    bool test_{false};
    // Use analytic interatomic forces rather than (production) tabulated potentials for tests
    bool testAnalytic_{false};
    // Include interatomic forces in test
    bool testInter_{true};
    // Include intramolecular forces in test
    bool testIntra_{true};
    // Threshold of force (%) at which test comparison will fail
    double testThreshold_{1.0e-6};
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
    bool setUp(Dissolve &dissolve, ProcessPool &procPool, Flags<KeywordBase::KeywordSignal> actionSignals) override;

    /*
     * Functions
     */
    public:
    // Calculate internal molecule forces, optionally only over the supplied molecules
    static void internalMoleculeForces(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                       bool includePairPotentialTerms, std::vector<Vec3<double>> &f,
                                       OptionalReferenceWrapper<std::vector<const Molecule *>> targetMolecules = std::nullopt);
    // Calculate pair potential forces within the specified Configuration
    static void pairPotentialForces(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                    std::vector<Vec3<double>> &f);
    // Calculate total forces within the specified Configuration
    static void totalForces(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                            std::vector<Vec3<double>> &f);
    // Calculate forces acting on specific Molecules within the specified Configuration (arising from all atoms)
    static void totalForces(ProcessPool &procPool, Configuration *cfg, const std::vector<const Molecule *> &targetMolecules,
                            const PotentialMap &potentialMap, std::vector<Vec3<double>> &f);
    // Calculate total forces within the specified Species
    static void totalForces(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap, std::vector<Vec3<double>> &f);
};
