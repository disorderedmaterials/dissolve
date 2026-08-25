// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/exportXYZConfiguration.h"
#include "classes/configuration.h"
#include "nodes/constants.h"
#include <fstream>

ExportXYZConfigurationNode::ExportXYZConfigurationNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Configuration to be exported", configuration_);

    // Options
    addOption("FilePath", "File path", filePath_);
    addOption("TagWithIteration", "Whether to tag (suffix) the filename with the current iteration index", tagWithIteration_);
    addSerialisable("Iteration", iteration_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ExportXYZConfigurationNode::type() const { return "ExportXYZConfiguration"; }

// Return short summary of the node's purpose
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

    exportConfiguration(configuration_, path);

    ++iteration_;

    return NodeConstants::ProcessResult::Success;
}

// Export the specified configuration
void ExportXYZConfigurationNode::exportConfiguration(const Configuration *cfg, std::string filePath)
{
    std::ofstream outfile(filePath);
    std::ostream_iterator<char> out(outfile);

    // Export number of atoms and title
    std::format_to(out, "{}\n", cfg->nAtoms());
    std::format_to(out, "{} @ {}\n", cfg->name(), cfg->version());

    // Export Atoms
    for (const auto &i : cfg->atoms())
        std::format_to(out, "{:<3}   {:15.9f}  {:15.9f}  {:15.9f}\n", Elements::symbol(i.speciesAtom()->Z()), i.r().x, i.r().y,
                       i.r().z);

    outfile.close();
}
