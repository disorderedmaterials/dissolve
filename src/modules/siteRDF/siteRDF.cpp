// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/siteRDF/siteRDF.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/fileAndFormat.h"
#include "keywords/optionalDouble.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "keywords/vec3Double.h"

SiteRDFModule::SiteRDFModule() : Module(ModuleTypes::SiteRDF)
{

    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<SpeciesSiteVectorKeyword>("SiteA", "Set the site(s) 'A' which are to represent the origin of the RDF", a_);
    keywords_.add<SpeciesSiteVectorKeyword>(
        "SiteB", "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated", b_);

    keywords_.setOrganisation("Options", "Ranges");
    keywords_.add<Vec3DoubleKeyword>("DistanceRange", "Range (min, max, delta) of distance axis", distanceRange_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxDeltaLabels);

    keywords_.setOrganisation("Options", "Control");
    keywords_.add<BoolKeyword>("ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule",
                               excludeSameMolecule_);

    keywords_.setOrganisation("Options", "Coordination Numbers");
    keywords_.add<BoolKeyword>("RangeAEnabled", "Whether calculation of the second coordination number is enabled",
                               rangeEnabled_[0]);
    keywords_.add<RangeKeyword>("RangeA", "Distance range for first coordination number", range_[0]);
    keywords_.add<BoolKeyword>("RangeBEnabled", "Whether calculation of the second coordination number is enabled",
                               rangeEnabled_[1]);
    keywords_.add<RangeKeyword>("RangeB", "Distance range for second coordination number", range_[1]);
    keywords_.add<BoolKeyword>("RangeCEnabled", "Whether calculation of the third coordination number is enabled",
                               rangeEnabled_[2]);
    keywords_.add<RangeKeyword>("RangeC", "Distance range for third coordination number", range_[2]);
    keywords_.add<BoolKeyword>("Instantaneous",
                               "Whether to calculate instantaneous coordination numbers rather than forming an average",
                               instantaneous_);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save calculated RDF data",
                                        exportFileAndFormat_, "EndExport");
    keywords_.add<BoolKeyword>("ExportInstantaneousCN", "Export instantaneous coordination numbers to disk\n",
                               exportInstantaneous_);
}
