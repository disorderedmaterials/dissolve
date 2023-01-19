// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/enumoptions.h"
#include "keywords/nodevector.h"
#include "math/data1d.h"
#include "math/data2d.h"
#include "math/data3d.h"
#include "procedure/nodes/select.h"

OperateNumberDensityNormaliseProcedureNode::OperateNumberDensityNormaliseProcedureNode(
    ConstNodeVector<SelectProcedureNode> nodes, SelectProcedureNode::SelectionPopulation populationTarget)
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateNumberDensityNormalise),
      normalisationSites_(std::move(nodes)), targetPopulation_{populationTarget}
{
    // Create keywords
    keywords_.add<NodeVectorKeyword<SelectProcedureNode>>("Control", "Site",
                                                          "Site(s) by which to normalise data based on their population",
                                                          normalisationSites_, this, ProcedureNode::NodeType::Select, false);
    keywords_.add<EnumOptionsKeyword<SelectProcedureNode::SelectionPopulation>>(
        "Control", "Population", "Target population quantity to use in normalisation", targetPopulation_,
        SelectProcedureNode::selectionPopulations());
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateNumberDensityNormaliseProcedureNode::operateData1D(const ProcessPool &procPool, Configuration *cfg)
{
    for (const auto node : normalisationSites_)
        if (targetPopulation_ == SelectProcedureNode::SelectionPopulation::Average)
            (*targetData1D_) /= (node->nAverageSites() / cfg->box()->volume());
        else if (targetPopulation_ == SelectProcedureNode::SelectionPopulation::Available)
            (*targetData1D_) /= (node->nAvailableSites() / cfg->box()->volume());

    return true;
}

// Operate on Data2D target
bool OperateNumberDensityNormaliseProcedureNode::operateData2D(const ProcessPool &procPool, Configuration *cfg)
{
    for (const auto node : normalisationSites_)
        if (targetPopulation_ == SelectProcedureNode::SelectionPopulation::Average)
            (*targetData2D_) /= (node->nAverageSites() / cfg->box()->volume());
        else if (targetPopulation_ == SelectProcedureNode::SelectionPopulation::Available)
            (*targetData2D_) /= (node->nAvailableSites() / cfg->box()->volume());

    return true;
}

// Operate on Data3D target
bool OperateNumberDensityNormaliseProcedureNode::operateData3D(const ProcessPool &procPool, Configuration *cfg)
{
    for (const auto node : normalisationSites_)
        if (targetPopulation_ == SelectProcedureNode::SelectionPopulation::Average)
            (*targetData3D_) /= (node->nAverageSites() / cfg->box()->volume());
        else if (targetPopulation_ == SelectProcedureNode::SelectionPopulation::Available)
            (*targetData3D_) /= (node->nAvailableSites() / cfg->box()->volume());

    return true;
}
