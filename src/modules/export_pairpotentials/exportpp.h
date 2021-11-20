// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/export/pairpotential.h"
#include "module/module.h"

// Export PairPotentials Module
class ExportPairPotentialsModule : public Module
{
    public:
    ExportPairPotentialsModule();
    ~ExportPairPotentialsModule() override = default;

    /*
     * Definition
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
