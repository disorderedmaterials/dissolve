// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "modules/clustering/clustering.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/expression.h"
#include "keywords/integer.h"
#include "keywords/speciesSite.h"

ClusteringModule::ClusteringModule() : Module(ModuleTypes::Clustering)
{
    executeIfTargetsUnchanged_ = true;

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});

    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<SpeciesSiteKeyword>("SiteA", "Choose the first site for cluster definition", a_);
    keywords_.add<SpeciesSiteKeyword>("SiteB", "Choose the second site for cluster definition", b_);
    keywords_.add<DoubleKeyword>(
        "Cut-off", "The maximum distance between sites for them to be considered part of the same cluster", cutoff_);

    keywords_.setOrganisation("Options", "Hydrogen Bonding");
    keywords_.add<BoolKeyword>("Directional Hydrogen Bonding?",
                               "Static sites based on (at least one) single hydroxyl oxygen atoms must be used", strict_);
    keywords_.add<DoubleKeyword>("Maximum bond angle deviation", "+- from 180 degrees", angleDev_);

    keywords_.setOrganisation("Export", "Options");
    keywords_.add<BoolKeyword>("Export Size Distribution", "SizeDist.txt", saveSizeDist_);
    keywords_.add<BoolKeyword>("Export Mass Distribution", "MassDist.txt", saveMassDist_);
    keywords_.add<BoolKeyword>("Export Fractal Dimension and Radius of Gyration - Cluster Mass", "RgMass.txt", saveRgMass_);
    keywords_.add<IntegerKeyword>("Minimum cluster size for Radius of Gyration and Fractal dimension", "", gyrationMinSize_);
}
