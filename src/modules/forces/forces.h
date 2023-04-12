// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

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
    // Use analytic forces rather than (production) tabulated potentials for tests
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
    bool process(Dissolve &dissolve, const ProcessPool &procPool) override;

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, const ProcessPool &procPool, Flags<KeywordBase::KeywordSignal> actionSignals) override;

    /*
     * Functions
     */
    public:
    // Force calculation type
    enum class ForceCalculationType
    {
        Full,
        PairPotentialOnly,
        IntraMolecularFull,
        IntraMolecularGeometry
    };
    // Calculate total forces within the specified Configuration
    static void totalForces(const ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                            ForceCalculationType calculationType, std::vector<Vec3<double>> &fInter,
                            std::vector<Vec3<double>> &fIntra, OptionalReferenceWrapper<Timer> commsTimer = {});
    // Calculate forces acting on specific Molecules within the specified Configuration (arising from all atoms)
    static void totalForces(const ProcessPool &procPool, Configuration *cfg,
                            const std::vector<const Molecule *> &targetMolecules, const PotentialMap &potentialMap,
                            ForceCalculationType calculationType, std::vector<Vec3<double>> &fInter,
                            std::vector<Vec3<double>> &fIntra, OptionalReferenceWrapper<Timer> commsTimer = {});
    // Calculate total forces within the specified Species
    static void totalForces(const ProcessPool &procPool, const Species *sp, const PotentialMap &potentialMap,
                            ForceCalculationType calculationType, std::vector<Vec3<double>> &fUnbound,
                            std::vector<Vec3<double>> &fBound,
                            OptionalReferenceWrapper<const std::vector<Vec3<double>>> r = {});
};
