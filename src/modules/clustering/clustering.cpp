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
    keywords_.add<BoolKeyword>("Self-clustering", "If SiteA and SiteB are different, A-A and B-B will contribute to clusters",
                               selfClustering_);

    keywords_.setOrganisation("Options", "Directional Hydrogen Bonding");
    keywords_.add<BoolKeyword>(
        "DirectionalHBonding",
        "Static and Dynamics sites should be based on a single atom/element/atom type, with hydrogens bonded to it. Fragment "
        "sites can be defined for more flexibility, requiring a #origin and #h tag for the origin atom and hydrogen (or other) "
        "group",
        directional_);
    keywords_.add<DoubleKeyword>("AngleTolerance", "Maximum angle deviation from 180 to be considered a valid interaction",
                                 angleDev_);

    keywords_.setOrganisation("Export", "Options");
    keywords_.add<BoolKeyword>("ExportSize", "Export size distribution to SizeDist.txt", saveSizeDist_);
    keywords_.add<BoolKeyword>("ExportMass", "Export mass distribution to MassDist.txt", saveMassDist_);
    keywords_.add<BoolKeyword>("ExportAnalysis", "Export fractal dimension and radius of gyration to RgMass.txt", saveRgMass_);
    keywords_.add<IntegerKeyword>(
        "MinGyrationSize", "Minimum cluster size to calculate radius of gyration and fractal dimension for", gyrationMinSize_);
}
