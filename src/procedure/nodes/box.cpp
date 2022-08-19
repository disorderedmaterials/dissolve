// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/box.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "keywords/bool.h"
#include "keywords/vec3nodevalue.h"

const Vec3<NodeValue> BoxProcedureNode::defaultAngles_ = {90.0, 90.0, 90.0};
const Vec3<NodeValue> BoxProcedureNode::defaultLengths_ = {1.0, 1.0, 1.0};

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
bool BoxProcedureNode::prepare(const ProcedureContext &procedureContext) { return true; }

// Execute node
bool BoxProcedureNode::execute(const ProcedureContext &procedureContext)
{
    // Create a Box in the target Configuration with our lengths and angles
    procedureContext.configuration()->createBox({lengths_.x.asDouble(), lengths_.y.asDouble(), lengths_.z.asDouble()},
                                                {angles_.x.asDouble(), angles_.y.asDouble(), angles_.z.asDouble()},
                                                nonPeriodic_);

    auto *box = procedureContext.configuration()->box();
    Messenger::print("[Box] Volume is {} cubic Angstroms (reciprocal volume = {:e})\n", box->volume(), box->reciprocalVolume());
    auto lengths = box->axisLengths();
    auto angles = box->axisAngles();
    Messenger::print(
        "[Box] Type is {}: A = {:10.4e} B = {:10.4e} C = {:10.4e}, alpha = {:10.4e} beta = {:10.4e} gamma = {:10.4e}\n",
        Box::boxTypes().keyword(box->type()), lengths.x, lengths.y, lengths.z, angles.x, angles.y, angles.z);

    return true;
}

// Express as a tree node
SerialisedValue BoxProcedureNode::serialise() const
{
    SerialisedValue box = ProcedureNode::serialise();
    if (lengths_ != defaultLengths_)
        box["lengths"] = lengths_;
    if (angles_ != defaultAngles_)
        box["angles"] = angles_;
    if (nonPeriodic_ != defaultNonPeriodic_)
        box["nonPeriodic"] = nonPeriodic_;
    return box;
}

void BoxProcedureNode::deserialise(const SerialisedValue &node, const CoreData &data)
{
    lengths_ = toml::find_or<>(node, "lengths", defaultLengths_);
    angles_ = toml::find_or<>(node, "angles", defaultAngles_);
    nonPeriodic_ = toml::find_or<bool>(node, "nonPeriodic", defaultNonPeriodic_);
}
