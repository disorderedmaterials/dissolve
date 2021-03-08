// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/export/pairpotential.h"
#include "module/module.h"

// Forward Declarations
/* none */

// Export PairPotentials Module
class ExportPairPotentialsModule : public Module
{
    /*
     * Export data in various formats
     */

    public:
    ExportPairPotentialsModule();
    ~ExportPairPotentialsModule() override = default;

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
    // Basename and format for PairPotential export
    PairPotentialExportFileFormat pairPotentialFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
