// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/exportXYZConfiguration.h"
#include "main/dissolve.h"
#include "nodes/constants.h"
#include <fstream>

ExportXYZConfigurationNode::ExportXYZConfigurationNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Configuration to be exported", configuration_);

    // Options
    addOption<std::string>("FilePath", "File path", filePath_);
    addOption<bool>("TagWithIteration", "Whether to tag (suffix) the filename with the current iteration index",
                    tagWithIteration_);
    addSerialisable("Iteration", iteration_);
}

/*
 * Definition
 */

std::string_view ExportXYZConfigurationNode::type() const { return "ExportXYZConfiguration"; }

std::string_view ExportXYZConfigurationNode::summary() const
{
    return "Export configuration coordinates from sequential frames of a coordinates.";
}

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ExportXYZConfigurationNode::process()
{
    auto path = filePath_;
    if (tagWithIteration_)
        path = std::format("{}.{}", path, iteration_);
    std::ofstream outfile(filePath_);
    std::ostream_iterator<char> out(outfile);

    // Export number of atoms and title
    std::format_to(out, "{}\n", configuration_->nAtoms());
    std::format_to(out, "{} @ {}\n", configuration_->name(), configuration_->version());

    // Export Atoms
    for (const auto &i : configuration_->atoms())
        std::format_to(out, "{:<3}   {:15.9f}  {:15.9f}  {:15.9f}\n", Elements::symbol(i.speciesAtom()->Z()), i.r().x, i.r().y,
                       i.r().z);

    outfile.close();

    ++iteration_;

    return NodeConstants::ProcessResult::Success;
}
