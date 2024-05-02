// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/dAngle/dAngle.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"

DAngleModule::DAngleModule() : Module(ModuleTypes::DAngle)
{
    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites", "Specify sites defining the angle interaction A-B...C");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Add site(s) which represent 'A' in the interaction A-B...C", a_);
    keywords_.add<SpeciesSiteVectorKeyword>("SiteB", "Add site(s) which represent 'B' in the interaction A-B...C", b_);
    keywords_.add<SpeciesSiteVectorKeyword>("SiteC", "Add site(s) which represent 'C' in the interaction A-B...C", c_);

    keywords_.setOrganisation("Options", "Ranges", "Ranges over which to bin quantities from the calculation.");
    keywords_.add<Vec3DoubleKeyword>("DistanceRange", "Range (min, max, binwidth) of distance binning", distanceRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("AngleRange", "Range (min, max, binwidth) of angle binning", angleRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);

    keywords_.setOrganisation("Options", "Exclusions", "Options to exclude specific pairs from the calculation.");
    keywords_.add<BoolKeyword>("ExcludeSameMolecule",
                               "Whether to exclude correlations between B and C sites on the same molecule",
                               excludeSameMolecule_);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("ExportRDF", "File format and file name under which to save calculated B-C RDF",
                                        exportFileAndFormatRDF_, "EndExportRDF");
    keywords_.add<FileAndFormatKeyword>("ExportAngle",
                                        "File format and file name under which to save calculated A-B...C angle histogram",
                                        exportFileAndFormatAngle_, "EndExportAngle");
    keywords_.add<FileAndFormatKeyword>("ExportDAngle",
                                        "File format and file name under which to save calculated A-B...C angle map",
                                        exportFileAndFormatDAngle_, "EndExportDAngle");
}
