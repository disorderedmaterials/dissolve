// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/cifRemoveAtomic.h"

CIFRemoveAtomicNode::CIFRemoveAtomicNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_)
        ->setFlags({ParameterBase::Required});

    // Outputs
    addOutput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_);

    // Options
    addOption<bool>("RemoveAtomics", "Whether to remove free atomic moieties in clean-up", removeAtomics_);
}

std::string_view CIFRemoveAtomicNode::type() const { return "CIFRemoveAtomic"; }

std::string_view CIFRemoveAtomicNode::summary() const { return "Remove atomics from a CIF context"; }

// Run main processing
NodeConstants::ProcessResult CIFRemoveAtomicNode::process()
{

    context_->setRemoveAtomics(removeAtomics_);

    return NodeConstants::ProcessResult::Success;
}
