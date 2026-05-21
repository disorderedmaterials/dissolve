// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/exportDLPOLYConfiguration.h"
#include "data/atomicMasses.h"
#include "main/dissolve.h"
#include "nodes/constants.h"
#include <fstream>

ExportDLPOLYConfigurationNode::ExportDLPOLYConfigurationNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Configuration to be exported", configuration_);

    // Options
    addOption<std::string>("FilePath", "File path", filePath_);
    addOption<bool>("TagWithIteration", "Whether to tag (suffix) the filename with the current iteration index",
                    tagWithIteration_);
}

std::string_view ExportDLPOLYConfigurationNode::type() const { return "ExportDLPOLYConfiguration"; }

std::string_view ExportDLPOLYConfigurationNode::summary() const { return "Export a configuration in DL_POLY format"; }

NodeConstants::ProcessResult ExportDLPOLYConfigurationNode::process()
{
    auto path = filePath_;
    if (tagWithIteration_)
        path = std::format("{}.{}", path, iteration_);

    std::ofstream outfile(filePath_);
    std::ostream_iterator<char> out(outfile);

    // Export title
    std::format_to(out, "{} @ {}\n", configuration_->name(), configuration_->version());

    // Export keytrj and imcon
    if (configuration_->box()->type() == Box::BoxType::NonPeriodic)
        std::format_to(out, "{:10d}{:10d}\n", 0, 0);
    else if (configuration_->box()->type() == Box::BoxType::Cubic)
        std::format_to(out, "{:10d}{:10d}\n", 0, 1);
    else if (configuration_->box()->type() == Box::BoxType::Orthorhombic)
        std::format_to(out, "{:10d}{:10d}\n", 0, 2);
    else
        std::format_to(out, "{:10d}{:10d}\n", 0, 3);

    // Export Cell
    if (configuration_->box()->type() != Box::BoxType::NonPeriodic)
    {
        Matrix3 axes = configuration_->box()->axes();
        std::format_to(out, "{:20.12f}{:20.12f}{:20.12f}\n", axes[0], axes[1], axes[2]);
        std::format_to(out, "{:20.12f}{:20.12f}{:20.12f}\n", axes[3], axes[4], axes[5]);
        std::format_to(out, "{:20.12f}{:20.12f}{:20.12f}\n", axes[6], axes[7], axes[8]);
    }

    // Export Atoms
    auto n = 0;
    for (const auto &i : configuration_->atoms())
        std::format_to(out, "{:<6}{:10d}{:20.10f}\n{:20.12f}{:20.12f}{:20.12f}\n", i.speciesAtom()->atomType()->name(), n++ + 1,
                       AtomicMass::mass(i.speciesAtom()->Z()), i.r().x, i.r().y, i.r().z);

    outfile.close();

    ++iteration_;

    return NodeConstants::ProcessResult::Success;
}
