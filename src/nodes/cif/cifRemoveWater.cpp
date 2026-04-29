// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/cif/cifRemoveWater.h"

CIFRemoveWaterNode::CIFRemoveWaterNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_)
        ->setFlags({ParameterBase::Required});

    // Outputs
    addOutput<CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_);

    // Options
    addOption<bool>("RemoveWaterAndCoordinatedOxygens", "Whether to remove water and coordinated oxygen atoms in clean-up",
                    removeWaterAndCoordinatedOxygens_);
}

std::string_view CIFRemoveWaterNode::type() const { return "CIFRemoveWater"; }

std::string_view CIFRemoveWaterNode::summary() const { return "Remove water from a CIF context"; }

// Run main processing
NodeConstants::ProcessResult CIFRemoveWaterNode::process()
{

    context_->setRemoveWaterAndCoordinateOxygens(removeWaterAndCoordinatedOxygens_);

    return NodeConstants::ProcessResult::Success;
}
