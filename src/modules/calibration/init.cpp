// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/calibration/calibration.h"

// Perform any necessary initialisation for the Module
void CalibrationModule::initialise()
{
    // Calculation
    keywords_.add("Calculation", new BoolKeyword(true), "OnlyWhenEnergyStable",
                  "Only perform calibrations when all related Configuration energies are stable");

    // RDF Calibration
    keywords_.add("RDF Calibration", new ModuleRefListKeyword(intraBroadeningModules_, std::vector<std::string>{"RDF"}),
                  "AdjustIntraBroadening", "Add specified RDF module as a target for IntraBroadening adjustment",
                  "<RDFModule>");

    // NeutronSQ Calibration
    keywords_.add("NeutronSQ Calibration",
                  new ModuleRefListKeyword(intraBroadeningNeutronGRReferences_, std::vector<std::string>{"NeutronSQ"}),
                  "IntraBroadeningNeutronGRReference",
                  "Add G(r) data in the specified NeutronSQ module as a reference for IntraBroadening adjustment",
                  "<NeutronSQModule>");
    keywords_.add("NeutronSQ Calibration",
                  new ModuleRefListKeyword(intraBroadeningNeutronSQReferences_, std::vector<std::string>{"NeutronSQ"}),
                  "IntraBroadeningNeutronSQReference",
                  "Add S(Q) data in the specified NeutronSQ module as a reference for IntraBroadening adjustment",
                  "<NeutronSQModule>");
}
