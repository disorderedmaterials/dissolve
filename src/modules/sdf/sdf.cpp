// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/sdf/sdf.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"
#include "procedure/nodes/calculateVector.h"
#include "procedure/nodes/collect3D.h"
#include "procedure/nodes/operateGridNormalise.h"
#include "procedure/nodes/operateSitePopulationNormalise.h"
#include "procedure/nodes/process3D.h"
#include "procedure/nodes/select.h"

SDFModule::SDFModule() : Module(ModuleTypes::SDF)
{
    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites", "Specify the central (A) and surrounding sites (B).");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Set the site(s) 'A' which are to represent the origin of the SDF", a_);
    keywords_.add<SpeciesSiteVectorKeyword>(
        "SiteB", "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated", b_);

    keywords_.setOrganisation("Options", "Ranges", "Ranges over which to bin quantities from the calculation.");
    keywords_.add<Vec3DoubleKeyword>("RangeX", "Range along X axis", rangeX_, Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                     Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<Vec3DoubleKeyword>("RangeY", "Range along Y axis", rangeY_, Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                     Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<Vec3DoubleKeyword>("RangeZ", "Range along Z axis", rangeZ_, Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                     Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels);

    keywords_.setOrganisation("Options", "Exclusions", "Options to exclude specific site pairs from the calculation.");
    keywords_.add<BoolKeyword>("ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule",
                               excludeSameMolecule_);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("ExportSDF", "Save the SDF to the specified file / format", sdfFileAndFormat_,
                                        "EndExportSDF");
}
