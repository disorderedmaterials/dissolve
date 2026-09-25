// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/exportDLPOLYConfiguration.h"
#include "classes/atomType.h"
#include "classes/configuration.h"
#include "data/atomicMasses.h"
#include "nodes/constants.h"
#include <fstream>

ExportDLPOLYConfigurationNode::ExportDLPOLYConfigurationNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Configuration to be exported", configuration_);

    // Options
    addOption("FilePath", "File path", filePath_);
    addOption("TagWithIteration", "Whether to tag (suffix) the filename with the current iteration index", tagWithIteration_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view ExportDLPOLYConfigurationNode::type() const { return "ExportDLPOLYConfiguration"; }

// Return short summary of the node's purpose
std::string_view ExportDLPOLYConfigurationNode::summary() const { return "Export a configuration in DL_POLY format"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ExportDLPOLYConfigurationNode::process()
{
    auto filePath = filePath_.string();
    auto path = filePath;
    if (tagWithIteration_)
        path = std::format("{}.{}", path, iteration_);

    exportConfiguration(configuration_, path);

    ++iteration_;

    return NodeConstants::ProcessResult::Success;
}

// Export the specified configuration
void ExportDLPOLYConfigurationNode::exportConfiguration(const Configuration *cfg, std::string filePath)
{
    std::ofstream outfile(filePath);
    std::ostream_iterator<char> out(outfile);

    // Export title
    std::format_to(out, "{} @ {}\n", cfg->name(), cfg->version());

    // Export keytrj and imcon
    if (cfg->box().type() == Box::BoxType::None)
        std::format_to(out, "{:10d}{:10d}\n", 0, 0);
    else if (cfg->box().type() == Box::BoxType::Cubic)
        std::format_to(out, "{:10d}{:10d}\n", 0, 1);
    else if (cfg->box().type() == Box::BoxType::Orthorhombic)
        std::format_to(out, "{:10d}{:10d}\n", 0, 2);
    else
        std::format_to(out, "{:10d}{:10d}\n", 0, 3);

    // Export Cell
    if (cfg->box().type() != Box::BoxType::None)
    {
        Matrix3 axes = cfg->box().axes();
        std::format_to(out, "{:20.12f}{:20.12f}{:20.12f}\n", axes[0], axes[1], axes[2]);
        std::format_to(out, "{:20.12f}{:20.12f}{:20.12f}\n", axes[3], axes[4], axes[5]);
        std::format_to(out, "{:20.12f}{:20.12f}{:20.12f}\n", axes[6], axes[7], axes[8]);
    }

    // Export Atoms
    auto n = 0;
    for (const auto &i : cfg->atoms())
        std::format_to(out, "{:<6}{:10d}{:20.10f}\n{:20.12f}{:20.12f}{:20.12f}\n", i.speciesAtom()->atomType()->name(), n++ + 1,
                       AtomicMass::mass(i.speciesAtom()->Z()), i.r().x, i.r().y, i.r().z);

    outfile.close();
}