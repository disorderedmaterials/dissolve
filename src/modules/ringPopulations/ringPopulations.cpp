// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/ringPopulations/ringPopulations.h"
#include "generator/select.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"

RingPopulationsModule::RingPopulationsModule() : Module(ModuleTypes::RingPopulations)
{
    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});

    keywords_.setOrganisation("Options", "Sites",
                              "Specify sites representing the bonding oxygen (BO) and network forming (NF) sites.");
    keywords_.add<SpeciesSiteVectorKeyword>("BondingOxygen", "Set the site(s) 'BO' which are to represent the bonding oxygen",
                                            bridgingOxygenSpeciesSites_);
    keywords_.add<SpeciesSiteVectorKeyword>(
        "NetworkFormer", "Set the site(s) 'NF' for which the distribution around the origin sites 'A' should be calculated",
        networkFormerSpeciesSites_);

    keywords_.setOrganisation("Options", "Ranges", "Ranges over which to bin quantities from the calculation.");
    keywords_.add<RangeKeyword>("DistanceRange",
                                "Distance range (min, max) over which to calculate Q-Species from central site", distanceRange_,
                                0.0, std::nullopt, Vec3Labels::MinMaxDeltaLabels);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save calculated QSpecies data",
                                        exportFileAndFormat_, "EndExport");
}
