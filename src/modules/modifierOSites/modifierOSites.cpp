// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/modifierOSites/modifierOSites.h"
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

ModifierOSitesModule::ModifierOSitesModule() : Module(ModuleTypes::ModifierOSites)
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
    keywords_.add<SpeciesSiteVectorKeyword>(
        "SiteA", "Set the modifier site(s) for which the distribution of oxygens should be calculated", modifierSpeciesSites_);

    keywords_.setOrganisation("Options", "Ranges", "Ranges over which to bin quantities from the calculation.");
    keywords_.add<RangeKeyword>("DistanceRange",
                                "Distance range (min, max) over which to calculate Q-Species from central site", distanceRange_,
                                0.0, std::nullopt, Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<RangeKeyword>("ModifierDistanceRange",
                                "Distance range (min, max) over which to calculate Modifier from central site",
                                modifierDistanceRange_, 0.0, std::nullopt, Vec3Labels::MinMaxDeltaLabels);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("ExportModifierData",
                                        "File format and file name under which to save calculated modifier to oxygen data",
                                        exportFileAndFormatOType_, "EndExportModifierData");
    keywords_.add<FileAndFormatKeyword>(
        "ExportOxygenTypes", "File format and file name under which to save calculated total oxygens to modifier atoms",
        exportFileAndFormatTotalOSites_, "EndExportOxygenTypes");
    keywords_.add<FileAndFormatKeyword>(
        "ExportFOLength", "File format and file name under which to save calculated free oxygens to modifier atom, bond length",
        exportFileAndFormatFOLength_, "EndExportFOLength");
    keywords_.add<FileAndFormatKeyword>(
        "ExportNBOLength",
        "File format and file name under which to save calculated non-bridging oxygens to modifier atom, bond length",
        exportFileAndFormatNBOLength_, "EndExportNBOLength");
    keywords_.add<FileAndFormatKeyword>(
        "ExportBOLength",
        "File format and file name under which to save calculated bridging oxygens to modifier atom, bond length",
        exportFileAndFormatBOLength_, "EndExportBOLength");
    keywords_.add<FileAndFormatKeyword>(
        "ExportOtherOLength",
        "File format and file name under which to save calculated other oxygens to modifier atom, bond length",
        exportFileAndFormatOtherOLength_, "EndExportOtherOLength");
}
