// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/histogramCN/histogramCN.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"

HistogramCNModule::HistogramCNModule() : Module(ModuleTypes::HistogramCN)
{
    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites", "Specify sites defining the interaction A...B.");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Set the site(s) 'A' which are to represent the reference origin", a_);
    keywords_.add<SpeciesSiteVectorKeyword>(
        "SiteB", "Set the site(s) 'B' for which the coordination number around the origin sites should be calculated", b_);

    keywords_.setOrganisation("Options", "Ranges", "Range over which to bin quantities from the calculation.");
    keywords_.add<RangeKeyword>("DistanceRange",
                                "Distance range (min, max) over which to calculate coordination number from central site",
                                distanceRange_, 0.0, std::nullopt, Vec3Labels::MinMaxDeltaLabels);
    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save calculated HitogramCN data",
                                        exportFileAndFormat_, "EndExport");
}
