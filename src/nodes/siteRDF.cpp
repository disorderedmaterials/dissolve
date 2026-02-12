// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/siteRDF.h"


SiteRDFNode::SiteRDFNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    // Options
    addOption("SiteA", "Set the site(s) 'A' which are to represent the origin of the RDF", a_);
    addOption(
        "SiteB", "Set the site(s) 'B' for which the distribution around the origin sites 'A' should be calculated", b_);
    addOption("DistanceRange", "Range (min, max, delta) of distance axis", distanceRange_);
    addOption("ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule",
                               excludeSameMolecule_);
    addOption("RangeAEnabled", "Whether calculation of the second coordination number is enabled",
                               rangeEnabled_[0]);
    addOption("RangeA", "Distance range for first coordination number", range_[0]);
    addOption("RangeBEnabled", "Whether calculation of the second coordination number is enabled",
                               rangeEnabled_[1]);
    addOption("RangeB", "Distance range for second coordination number", range_[1]);
    addOption("RangeCEnabled", "Whether calculation of the third coordination number is enabled",
                               rangeEnabled_[2]);
    addOption("RangeC", "Distance range for third coordination number", range_[2]);
    addOption("Instantaneous",
                               "Whether to calculate instantaneous coordination numbers rather than forming an average",
                               instantaneous_);
    addOption("Export", "File format and file name under which to save calculated RDF data",
                                        exportFileAndFormat_);
    addOption("ExportInstantaneousCN", "Export instantaneous coordination numbers to disk\n",
                               exportInstantaneous_);
}

std::string_view SiteRDFNode::type() const { return "SiteRDF"; }

std::string_view SiteRDFNode::summary() const { return "Calculate a site-site radial distribution function and associated coordination numbers"; }

NodeConstants::ProcessResult SiteRDFNode::process()
{
    return NodeConstants::ProcessResult::Unchanged;
}