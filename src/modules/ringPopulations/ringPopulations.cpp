// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/ringPopulations/ringPopulations.h"
#include "generator/select.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/optionalInt.h"
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

    keywords_.setOrganisation("Options", "Sites", "Specify sites representing the ring former and bidging site.");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Set the site(s) which represent the ring formers", a_);
    keywords_.add<SpeciesSiteVectorKeyword>("SiteB", "Set the site(s) which represents the bridging sites", b_);

    keywords_.setOrganisation("Options", "Ranges", "Ranges over which to bin quantities from the calculation.");
    keywords_.add<RangeKeyword>("DistanceRange",
                                "Distance range (min, max) over which to calculate Q-Species from central site", distanceRange_,
                                0.0, std::nullopt, Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<OptionalIntegerKeyword>("Max Ring Size", "Maximum Ring Size to compute", maxRingSize_, 1, std::nullopt, 1,
                                          "Off");

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save calculated QSpecies data",
                                        exportFileAndFormat_, "EndExport");
}
