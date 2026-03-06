// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/cifNETAOptions.h"

CIFNETAOptionsNode::CIFNETAOptionsNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_);

    // Outputs
    addOutput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_);

    // Options
    addOption<bool>("RemoveNETA", "Whether to remove by NETA definition in clean-up", removeNETA_);
    addOption<bool>("removeNETAByFragment", "Whether to expand NETA matches to fragments when removing in clean-up",
                    removeNETAByFragment_);
    addOption<std::string>("MoietyRemovalNETA", "NETA for moiety removal", moietyRemovalNETA_);
}

std::string_view CIFNETAOptionsNode::type() const { return "CIFNETAOptions"; }

std::string_view CIFNETAOptionsNode::summary() const { return "Apply NETA options to a CIF context"; }

// Run main processing
NodeConstants::ProcessResult CIFNETAOptionsNode::process()
{
    if (!context_)
    {
        error("CIF context is null");
        return NodeConstants::ProcessResult::Failed;
    }

    context_->setRemoveNETA(removeNETA_, removeNETAByFragment_);
    context_->setMoietyRemovalNETA(std::string_view(moietyRemovalNETA_));

    return NodeConstants::ProcessResult::Success;
}
