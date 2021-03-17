// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/calibration/calibration.h"

// Perform any necessary initialisation for the Module
void CalibrationModule::initialise()
{
    // Control
    keywords_.add("Control", new BoolKeyword(true), "OnlyWhenEnergyStable",
                  "Only perform calibrations when all related Configuration energies are stable");

    // RDF Calibration
    keywords_.add("RDF Calibration", new ModuleVectorKeyword({"RDF"}), "AdjustIntraBroadening",
                  "Add specified RDF module as a target for IntraBroadening adjustment", "<RDFModule>");

    // NeutronSQ Calibration
    keywords_.add("NeutronSQ Calibration", new ModuleVectorKeyword({"NeutronSQ"}), "IntraBroadeningNeutronGRReference",
                  "Add G(r) data in the specified NeutronSQ module as a reference for IntraBroadening adjustment",
                  "<NeutronSQModule>");
    keywords_.add("NeutronSQ Calibration", new ModuleVectorKeyword({"NeutronSQ"}), "IntraBroadeningNeutronSQReference",
                  "Add S(Q) data in the specified NeutronSQ module as a reference for IntraBroadening adjustment",
                  "<NeutronSQModule>");
}
