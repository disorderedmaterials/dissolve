// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/intraAngle/intraAngle.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"

IntraAngleModule::IntraAngleModule() : Module(ModuleTypes::IntraAngle)
{
    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites", "Specify sites defining the angle interaction A-B-C.");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Specify site(s) which represent 'A' in the interaction A-B-C", a_);
    keywords_.add<SpeciesSiteVectorKeyword>("SiteB", "Specify site(s) which represent 'B' in the interaction A-B-C", b_);
    keywords_.add<SpeciesSiteVectorKeyword>("SiteC", "Specify site(s) which represent 'C' in the interaction A-B-C", c_);

    keywords_.setOrganisation("Options", "Ranges", "Ranges over which to bin quantities from the calculation.");
    keywords_.add<RangeKeyword>("RangeAB", "Range (min, max) of A-B distance binning", rangeAB_, 0.0,
                                Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<RangeKeyword>("RangeBC", "Range (min, max, binwidth) of B-C distance binning", rangeBC_, 0.0,
                                Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("AngleRange", "Range (min, max, binwidth) of angle binning", angleRange_,
                                     Vec3<double>(0.0, 180.0, 1.0), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);

    keywords_.setOrganisation("Options", "Restrictions", "Options to restrict the angle range considered.");
    keywords_.add<BoolKeyword>(
        "Symmetric", "Whether the calculated angle should be mapped to 0 - 90 (i.e. is symmetric about 90)", symmetric_);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("ExportAngle",
                                        "File format and file name under which to save calculated A-B-C angle histogram",
                                        exportFileAndFormat_, "EndExportAngle");
}
