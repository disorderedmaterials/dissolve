// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/exportXYZTrajectory.h"
#include <fstream>

ExportXYZTrajectoryNode::ExportXYZTrajectoryNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Configuration from which Trajectory will be exported", configuration_);

    // Options
    addOption<std::string>("FilePath", "File path", filePath_);
    addOption<bool>("Extended", "Extended XYZ format", extended_);
}

/*
 * Definition
 */

std::string_view ExportXYZTrajectoryNode::type() const { return "ExportTrajectory"; }

std::string_view ExportXYZTrajectoryNode::summary() const
{
    return "Export configuration coordinates as sequential frames of a trajectory.";
}

/*
 * Processing
 */

// Run main processing
NodeConstants::ProcessResult ExportXYZTrajectoryNode::process()
{
    std::ofstream outfile(filePath_);
    std::ostream_iterator<char> out(outfile);

    std::format_to(out, "{}\n", configuration_->nAtoms());
    std::format_to(out, "{} @ {}\n", configuration_->name(), configuration_->version());

    for (const auto &i : configuration_->atoms())
    {
        if (extended_)
            std::format_to(out, "{:<3}   {:15.9f}  {:15.9f}  {:15.9f}  {:<6d}  {}\n", Elements::symbol(i.speciesAtom()->Z()),
                           i.r().x, i.r().y, i.r().z, i.speciesAtom()->index() + 1, i.speciesAtom()->atomType()->name());
        else
            std::format_to(out, "{:<3}   {:15.9f}  {:15.9f}  {:15.9f}\n", Elements::symbol(i.speciesAtom()->Z()), i.r().x,
                           i.r().y, i.r().z);
    }
    outfile.close();

    return NodeConstants::ProcessResult::Success;
}
