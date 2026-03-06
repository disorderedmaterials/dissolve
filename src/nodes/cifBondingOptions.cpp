// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/cifBondingOptions.h"

CIFBondingOptionsNode::CIFBondingOptionsNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_);

    // Outputs
    addOutput<CIFLoaderNode::CIFContext *>("CIFContext", "CIF handling context derived from parsing of CIF file", context_);

    // Options
    addOption<Number>("BondingTolerance", "Bonding tolerance, if calculating bonding rather than using CIF definitions",
                      bondingTolerance_);
    addOption<bool>("UseCIFBondingDefinitions", "Whether to use CIF bonding definitions", useCIFBondingDefinitions_);
    addOption<bool>("PreventMetallicBonds", "Whether to prevent metallic bonding", preventMetallicBonds_);
}

std::string_view CIFBondingOptionsNode::type() const { return "CIFBondingOptions"; }

std::string_view CIFBondingOptionsNode::summary() const { return "Apply bonding options to a CIF context"; }

// Run main processing
NodeConstants::ProcessResult CIFBondingOptionsNode::process()
{
    if (!context_)
    {
        error("CIF context is null");
        return NodeConstants::ProcessResult::Failed;
    }

    context_->setBondingTolerance(bondingTolerance_.asDouble());
    context_->setUseCIFBondingDefinitions(useCIFBondingDefinitions_);
    context_->setPreventMetallicBonds(preventMetallicBonds_);

    return NodeConstants::ProcessResult::Success;
}
