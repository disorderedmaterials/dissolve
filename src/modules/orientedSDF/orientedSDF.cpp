// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/orientedSDF/orientedSDF.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"

OrientedSDFModule::OrientedSDFModule() : Module(ModuleTypes::OrientedSDF)
{
    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites", "Specify the central (A) and surrounding sites (B).");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Set the site(s) 'A' which are to represent the origin of the SDF", a_,
                                            true);
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("AxisA", "Axis to use from site A", axisA_,
                                                              OrientedSite::siteAxis());
    keywords_.add<SpeciesSiteVectorKeyword>(
        "SiteB", "Set the site(s) 'B' for which the distribution around the origin site(s) 'A' should be calculated", b_);
    keywords_.add<EnumOptionsKeyword<OrientedSite::SiteAxis>>("AxisB", "Axis to use from site B", axisB_,
                                                              OrientedSite::siteAxis());

    keywords_.setOrganisation("Options", "Ranges", "Ranges over which to bin quantities from the calculation.");
    keywords_.add<RangeKeyword>("AngleRange", "Axis angle range required to permit a site to be binned in the SDF",
                                axisAngleRange_, Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<Vec3DoubleKeyword>("RangeX", "Range along X axis", rangeX_, Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                     Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<Vec3DoubleKeyword>("RangeY", "Range along Y axis", rangeY_, Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                     Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<Vec3DoubleKeyword>("RangeZ", "Range along Z axis", rangeZ_, Vec3<double>(-1.0e6, -1.0e6, 0.05),
                                     Vec3<double>(1.0e6, 1.0e6, 1.0e4), Vec3Labels::MinMaxDeltaLabels);

    keywords_.setOrganisation(
        "Options", "Exclusions & Restrictions",
        "Options to exclude specific site pairs from the calculation and restrict the angle range considered.");
    keywords_.add<BoolKeyword>("ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule",
                               excludeSameMolecule_);
    keywords_.add<BoolKeyword>(
        "Symmetric", "Whether the calculated angle should be mapped to 0 - 90 (i.e. is symmetric about 90)", symmetric_);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("ExportSDF", "Save the SDF to the specified file / format", sdfFileAndFormat_,
                                        "EndExportSDF");
}
