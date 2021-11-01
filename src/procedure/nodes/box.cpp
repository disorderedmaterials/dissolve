// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/box.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "keywords/types.h"

BoxProcedureNode::BoxProcedureNode(Vec3<NodeValue> lengths, Vec3<NodeValue> angles, bool nonPeriodic)
    : ProcedureNode(ProcedureNode::NodeType::Box), angles_(std::move(angles)), lengths_(std::move(lengths)),
      nonPeriodic_(nonPeriodic)
{
    keywords_.add<Vec3NodeValueKeyword>("Control", "Lengths", "Box lengths", lengths_, this, Vec3Labels::ABCLabels);
    keywords_.add<Vec3NodeValueKeyword>("Control", "Angles", "Box angles", angles_, this, Vec3Labels::AlphaBetaGammaLabels);
    keywords_.add<BoolKeyword>("Control", "NonPeriodic", "Whether the box is non-periodic", nonPeriodic_);
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool BoxProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::GenerationContext);
}

// Return whether a name for the node must be provided
bool BoxProcedureNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool BoxProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) { return true; }

// Execute node, targetting the supplied Configuration
bool BoxProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Retrieve necessary parameters
    auto lengths = keywords_.asVec3Double("Lengths");
    auto angles = keywords_.asVec3Double("Angles");

    // Create a Box in the target Configuration with our lengths and angles
    cfg->createBox(lengths, angles, nonPeriodic_);

    Messenger::print("[Box] Volume is {} cubic Angstroms (reciprocal volume = {:e})\n", cfg->box()->volume(),
                     cfg->box()->reciprocalVolume());
    lengths = cfg->box()->axisLengths();
    angles = cfg->box()->axisAngles();
    Messenger::print(
        "[Box] Type is {}: A = {:10.4e} B = {:10.4e} C = {:10.4e}, alpha = {:10.4e} beta = {:10.4e} gamma = {:10.4e}\n",
        Box::boxTypes().keyword(cfg->box()->type()), lengths.x, lengths.y, lengths.z, angles.x, angles.y, angles.z);

    return true;
}
