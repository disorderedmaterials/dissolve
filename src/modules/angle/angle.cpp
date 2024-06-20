// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/angle/angle.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"
#include "module/context.h"

AngleModule::AngleModule() : Module(ModuleTypes::Angle)
{
    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});

    keywords_.setOrganisation("Options", "Sites", "Specify sites defining the angle interaction A-B-C.");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Specify site(s) which represent 'A' in the interaction A-B-C", a_);
    keywords_.add<SpeciesSiteVectorKeyword>("SiteB", "Specify site(s) which represent 'B' in the interaction A-B-C", b_);
    keywords_.add<SpeciesSiteVectorKeyword>("SiteC", "Specify site(s) which represent 'C' in the interaction A-B-C", c_);

    keywords_.setOrganisation("Options", "Ranges", "Ranges over which to bin quantities from the calculation.");
    keywords_.add<Vec3DoubleKeyword>("RangeAB", "Range (min, max, binwidth) of A-B distance binning", rangeAB_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("RangeBC", "Range (min, max, binwidth) of B-C distance binning", rangeBC_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("AngleRange", "Range (min, max, binwidth) of angle binning", angleRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);

    keywords_.setOrganisation(
        "Options", "Exclusions & Restrictions",
        "Options to exclude specific site pairs from the calculation and restrict the angle range considered.");
    keywords_.add<BoolKeyword>("ExcludeSameMoleculeAB",
                               "Whether to exclude correlations between A and B sites on the same molecule",
                               excludeSameMoleculeAB_);
    keywords_.add<BoolKeyword>("ExcludeSameMoleculeBC",
                               "Whether to exclude correlations between B and C sites on the same molecule",
                               excludeSameMoleculeBC_);
    keywords_.add<BoolKeyword>(
        "ExcludeSameSiteAC", "Whether to exclude correlations between A and C sites on the same molecule", excludeSameSiteAC_);
    keywords_.add<BoolKeyword>(
        "Symmetric", "Whether the calculated angle should be mapped to 0 - 90 (i.e. is symmetric about 90)", symmetric_);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("ExportAB", "File format and file name under which to save calculated A-B RDF data",
                                        exportFileAndFormatAB_, "EndExportAB");
    keywords_.add<FileAndFormatKeyword>("ExportBC", "File format and file name under which to save calculated B-C RDF data",
                                        exportFileAndFormatBC_, "EndExportBC");
    keywords_.add<FileAndFormatKeyword>("ExportAngle",
                                        "File format and file name under which to save calculated A-B-C angle histogram",
                                        exportFileAndFormatAngle_, "EndExportAngle");
    keywords_.add<FileAndFormatKeyword>("ExportDAngleAB",
                                        "File format and file name under which to save calculated (A-B)-C distance-angle map",
                                        exportFileAndFormatDAngleAB_, "EndExportDAngleAB");
    keywords_.add<FileAndFormatKeyword>("ExportDAngleBC",
                                        "File format and file name under which to save calculated A-(B-C) distance-angle map",
                                        exportFileAndFormatDAngleBC_, "EndExportDAngleBC");
}
