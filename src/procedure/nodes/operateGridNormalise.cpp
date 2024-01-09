// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/operateGridNormalise.h"
#include "base/lineParser.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "procedure/nodes/select.h"

OperateGridNormaliseProcedureNode::OperateGridNormaliseProcedureNode()
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateGridNormalise)
{
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateGridNormaliseProcedureNode::operateData1D(const ProcessPool &procPool, Configuration *cfg)
{
    if (targetData1D_->xAxis().size() < 2)
        return Messenger::error("Not enough data to determine 1D grid spacing for normalisation.\n");

    // Determine bin width from first points of data
    double xBinWidth = targetData1D_->xAxis().at(1) - targetData1D_->xAxis().at(0);
    Messenger::print("OperateGridNormalise - Grid point spacing is {:e}.\n", xBinWidth);

    (*targetData1D_) /= xBinWidth;

    return true;
}

// Operate on Data2D target
bool OperateGridNormaliseProcedureNode::operateData2D(const ProcessPool &procPool, Configuration *cfg)
{
    if (targetData2D_->xAxis().size() < 2)
        return Messenger::error("Not enough data to determine 2D grid spacing for normalisation.\n");
    if (targetData2D_->yAxis().size() < 2)
        return Messenger::error("Not enough data to determine 2D grid spacing for normalisation.\n");

    // Determine bin area from first points of data
    double xBinWidth = targetData2D_->xAxis().at(1) - targetData2D_->xAxis().at(0);
    double yBinWidth = targetData2D_->yAxis().at(1) - targetData2D_->yAxis().at(0);
    double binArea = xBinWidth * yBinWidth;
    Messenger::print("OperateGridNormalise - Grid pixel area is {:e}.\n", binArea);

    (*targetData2D_) /= binArea;

    return true;
}

// Operate on Data3D target
bool OperateGridNormaliseProcedureNode::operateData3D(const ProcessPool &procPool, Configuration *cfg)
{
    if (targetData3D_->xAxis().size() < 2)
        return Messenger::error("Not enough data to determine 3D grid spacing for normalisation.\n");
    if (targetData3D_->yAxis().size() < 2)
        return Messenger::error("Not enough data to determine 3D grid spacing for normalisation.\n");
    if (targetData3D_->zAxis().size() < 2)
        return Messenger::error("Not enough data to determine 3D grid spacing for normalisation.\n");

    // Determine bin area from first points of data
    double xBinWidth = targetData3D_->xAxis().at(1) - targetData3D_->xAxis().at(0);
    double yBinWidth = targetData3D_->yAxis().at(1) - targetData3D_->yAxis().at(0);
    double zBinWidth = targetData3D_->zAxis().at(1) - targetData3D_->zAxis().at(0);
    double binVolume = xBinWidth * yBinWidth * zBinWidth;
    Messenger::print("OperateGridNormalise - Grid voxel volume is {:e}.\n", binVolume);

    (*targetData3D_) /= binVolume;

    return true;
}
