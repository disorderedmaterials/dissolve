// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/operateNumberDensityNormalise.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/enumOptions.h"
#include "keywords/nodeVector.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "procedure/nodes/select.h"

OperateNumberDensityNormaliseProcedureNode::OperateNumberDensityNormaliseProcedureNode(
    ConstNodeVector<SelectProcedureNode> nodes, SelectProcedureNode::SelectionPopulation populationTarget)
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateNumberDensityNormalise),
      normalisationSites_(std::move(nodes)), targetPopulation_{populationTarget}
{
    keywords_.setOrganisation("Options", "Inputs");
    keywords_.add<NodeVectorKeyword<SelectProcedureNode>>("Site",
                                                          "Site(s) by which to normalise data based on their population",
                                                          normalisationSites_, this, ProcedureNode::NodeType::Select, false);
    keywords_.add<EnumOptionsKeyword<SelectProcedureNode::SelectionPopulation>>(
        "Population", "Target population quantity to use in normalisation", targetPopulation_,
        SelectProcedureNode::selectionPopulations());
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateNumberDensityNormaliseProcedureNode::operateData1D(const ProcessPool &procPool, Configuration *cfg)
{
    for (const auto &node : normalisationSites_)
        if (targetPopulation_ == SelectProcedureNode::SelectionPopulation::Average)
            (*targetData1D_) /= (node->nAverageSites() / cfg->box()->volume());
        else if (targetPopulation_ == SelectProcedureNode::SelectionPopulation::Available)
            (*targetData1D_) /= (node->nAvailableSitesAverage() / cfg->box()->volume());

    return true;
}

// Operate on Data2D target
bool OperateNumberDensityNormaliseProcedureNode::operateData2D(const ProcessPool &procPool, Configuration *cfg)
{
    for (const auto &node : normalisationSites_)
        if (targetPopulation_ == SelectProcedureNode::SelectionPopulation::Average)
            (*targetData2D_) /= (node->nAverageSites() / cfg->box()->volume());
        else if (targetPopulation_ == SelectProcedureNode::SelectionPopulation::Available)
            (*targetData2D_) /= (node->nAvailableSitesAverage() / cfg->box()->volume());

    return true;
}

// Operate on Data3D target
bool OperateNumberDensityNormaliseProcedureNode::operateData3D(const ProcessPool &procPool, Configuration *cfg)
{
    for (const auto &node : normalisationSites_)
        if (targetPopulation_ == SelectProcedureNode::SelectionPopulation::Average)
            (*targetData3D_) /= (node->nAverageSites() / cfg->box()->volume());
        else if (targetPopulation_ == SelectProcedureNode::SelectionPopulation::Available)
            (*targetData3D_) /= (node->nAvailableSitesAverage() / cfg->box()->volume());

    return true;
}
