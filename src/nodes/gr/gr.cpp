// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/gr/gr.h"

GRNode::GRNode(Graph *parentGraph)
    : Node(parentGraph), rawGRHistory_(
                             [&]()
                             {
                                 PartialSet p;
                                 p.initialise(targetConfiguration_->speciesPopulations());
                                 return p;
                             })
{
    // Inputs
    addInput<Configuration *>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    // Outputs
    addOptionalPointerOutput<PartialSet>("UnweightedGR", "Unweighted partials for target configuration", unweightedGR_);

    // Options
    addOption<Number>("BinWidth", "Bin width (spacing in r) to use", binWidth_);
    addOption<std::optional<Number>>("Range", "Maximum r to calculate g(r) out to", requestedRange_);
    addOption<std::optional<Number>>("Averaging", "Number of historical partial sets to combine into final partials",
                                     averagingLength_);
    addOption<Function1DWrapper>("IntraBroadening", "Type of broadening to apply to intramolecular g(r)", intraBroadening_);
    addOption<std::optional<Number>>("Smoothing", "Specifies the degree of smoothing to apply to calculated g(r)", nSmooths_);
    addOption<bool>("Save", "Whether to save partials and total functions to disk", save_);
    addOption<bool>("SaveRaw", "Whether to save raw simulation partial and total functions to disk", saveRaw_);
    addOption<bool>(
        "InternalTest",
        "Perform internal check of calculated partials against a set calculated by a simple unoptimised double-loop",
        internalTest_);
    addOption<GRNode::PartialsMethod>("Method", "Calculation method for partial radial distribution functions",
                                      partialsMethod_);
    addOptionalPointerOutput<PartialSet>("RawGR", "Origin (unbroadened) partials for the target configuration", rawGR_);
    addOptionalPointerOutput<PartialSet>("UnweightedGR", "Unweighted partials for target configuration", unweightedGR_);

    // Serialisables
    addSerialisable("rawGR", rawGR_);
    addSerialisable("rawGRHistory", rawGRHistory_);
    addSerialisable("unweightedGR", unweightedGR_);
}

// Return enum option info for NormalisationType
EnumOptions<GRNode::PartialsMethod> GRNode::partialsMethods()
{
    return EnumOptions<GRNode::PartialsMethod>("PartialsMethod", {{PartialsMethod::AutoMethod, "Auto"},
                                                                  {PartialsMethod::CellsMethod, "Cells"},
                                                                  {PartialsMethod::SimpleMethod, "Simple"},
                                                                  {PartialsMethod::TestMethod, "Test"}});
}

std::string_view GRNode::type() const { return "GR"; }

std::string_view GRNode::summary() const { return "Calculate radial distribution functions between all atom types."; }

EnumOptions<GRNode::PartialsMethod> getEnumOptions(GRNode::PartialsMethod) { return GRNode::partialsMethods(); }
