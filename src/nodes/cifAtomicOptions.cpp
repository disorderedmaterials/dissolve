// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/cifAtomicOptions.h"

CIFAtomicOptionsNode::CIFAtomicOptionsNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_);

    // Outputs
    addOutput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_);

    // Options
    addOption<Number>("OverlapTolerance", "Tolerance for removal of overlapping atoms", overlapTolerance_);
    addOption<bool>("RemoveAtomics", "Whether to remove free atomic moieties in clean-up", removeAtomics_);
    addOption<bool>("RemoveWaterAndCoordinateOxygens", "Whether to remove water and coordinated oxygen atoms in clean-up",
                    removeWaterAndCoordinateOxygens_);
}

std::string_view CIFAtomicOptionsNode::type() const { return "CIFAtomicOptions"; }

std::string_view CIFAtomicOptionsNode::summary() const { return "Apply atomic options to a CIF context"; }

// Run main processing
NodeConstants::ProcessResult CIFAtomicOptionsNode::process()
{
    if (!context_)
    {
        error("CIF context is null");
        return NodeConstants::ProcessResult::Failed;
    }

    context_->setOverlapTolerance(overlapTolerance_.asDouble());
    context_->setRemoveAtomics(removeAtomics_);
    context_->setRemoveWaterAndCoordinateOxygens(removeWaterAndCoordinateOxygens_);

    return NodeConstants::ProcessResult::Success;
}
