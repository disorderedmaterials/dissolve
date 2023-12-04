// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/qSpecies/qSpecies.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"
#include "procedure/nodes/calculateExpression.h"
#include "procedure/nodes/ifValueInRange.h"
#include "procedure/nodes/integerCollect1D.h"
#include "procedure/nodes/iterateData1D.h"
#include "procedure/nodes/operateNormalise.h"
#include "procedure/nodes/process1D.h"
#include "procedure/nodes/select.h"

QSpeciesModule::QSpeciesModule() : Module(ModuleTypes::QSpecies)
{
    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites",
                              "Specify sites representing the bonding oxygen (BO) and network forming (NF) sites.");
    keywords_.add<SpeciesSiteVectorKeyword>("BondingOxygen", "Set the site(s) 'BO' which are to represent the bonding oxygen",
                                            bridgingOxygenSites_);
    keywords_.add<SpeciesSiteVectorKeyword>(
        "NetworkFormer", "Set the site(s) 'NF' for which the distribution around the origin sites 'A' should be calculated",
        networkFormerSites_);

    keywords_.setOrganisation("Options", "Ranges", "Ranges over which to bin quantities from the calculation.");
    keywords_.add<RangeKeyword>("DistanceRange",
                                "Distance range (min, max) over which to calculate Q-Species from central site", distanceRange_,
                                0.0, std::nullopt, Vec3Labels::MinMaxDeltaLabels);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save calculated QSpecies data",
                                        exportFileFormat_, "EndExport");
}
