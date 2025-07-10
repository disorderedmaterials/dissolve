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

    keywords_.setOrganisation("Options", "Analysis", "Hover for more details on settings");
    keywords_.add<SpeciesSiteKeyword>("SiteA", "Choose the first site for cluster definition", a_);
    keywords_.add<SpeciesSiteKeyword>("SiteB", "Choose the second site for cluster definition", b_);
    keywords_.add<DoubleKeyword>(
        "Cutoff", "The maximum distance between sites for them to be considered part of the same cluster", cutoff_);
    keywords_.add<BoolKeyword>("IncludeAA", "If SiteA and SiteB are different, A-A interactions will contribute to clusters",
                               selfClusteringA_);
    keywords_.add<BoolKeyword>("IncludeBB", "If SiteA and SiteB are different, B-B interactions will contribute to clusters",
                               selfClusteringB_);
    keywords_.add<DoubleKeyword>("FisherExponent",
                                 "Exponent determining slope of percolation line in the distribution plot (default = -2.189)",
                                 fisher_);

    keywords_.setOrganisation(
        "Options", "Directional Hydrogen Bonding",
        "Static and Dynamics sites should be based on a single atom/element/atom type, with hydrogens bonded to it. Fragment "
        "sites can be defined for more flexibility, requiring a #origin and #other tag for the origin atom and hydrogen (or "
        "other) group - See documentation for more detail");
    keywords_.add<BoolKeyword>("DirectionalBonding", "Perform directionality checks between sites", directional_);
    keywords_.add<DoubleKeyword>("MaxAngleTolerance", "Maximum angle deviation from 180 to be considered a valid interaction",
                                 maxAngleDev_);
    keywords_.add<DoubleKeyword>("MinAngleTolerance", "Minimum angle deviation from 180 to be considered a valid interaction",
                                 minAngleDev_);

    keywords_.setOrganisation("Export", "Options",
                              "Each iteration the data for just that pass is calculated and printed to the relevant file.");
    keywords_.add<BoolKeyword>("ExportSnapshotSize", "Export instantaneous size distribution to ...SizeDist.txt",
                               saveSizeDist_);
    keywords_.add<BoolKeyword>("ExportSnapshotMass", "Export instantaneous mass distribution to ...MassDist.txt",
                               saveMassDist_);
    keywords_.add<BoolKeyword>("ExportSnapshotAnalysis", "Export fractal dimension and radius of gyration to ...RgMass.txt",
                               saveRgMass_);
    keywords_.add<BoolKeyword>("ExportSnapshotContacts", "Export contact numbers to ...CN.txt", saveCN_);
    keywords_.add<IntegerKeyword>(
        "MinGyrationSize", "Minimum cluster size to calculate radius of gyration and fractal dimension for", gyrationMinSize_);
}
