// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/operateSphericalShellNormalise.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "math/data2D.h"
#include "procedure/nodes/select.h"

OperateSphericalShellNormaliseProcedureNode::OperateSphericalShellNormaliseProcedureNode()
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateSphericalShellNormalise)
{
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateSphericalShellNormaliseProcedureNode::operateData1D(const ProcessPool &procPool, Configuration *cfg)
{
    // We expect x values to be centre-bin values, and regularly spaced
    const auto &xAxis = targetData1D_->xAxis();
    auto &values = targetData1D_->values();
    if (xAxis.size() < 2)
        return true;

    // Derive first left-bin boundary from the delta between points 0 and 1
    double leftBin = xAxis[0] - (xAxis[1] - xAxis[0]) * 0.5, rightBin, divisor;
    double r1Cubed = pow(leftBin, 3), r2Cubed;
    for (auto n = 0; n < xAxis.size(); ++n)
    {
        // Get new right-bin from existing left bin boundary and current bin centre
        rightBin = leftBin + 2 * (xAxis[n] - leftBin);
        r2Cubed = pow(rightBin, 3);
        divisor = (4.0 / 3.0) * PI * (r2Cubed - r1Cubed);
        values[n] /= divisor;
        if (targetData1D_->valuesHaveErrors())
            targetData1D_->error(n) /= divisor;

        // Overwrite old values
        r1Cubed = r2Cubed;
        leftBin = rightBin;
    }

    return true;
}

// Operate on Data2D target
bool OperateSphericalShellNormaliseProcedureNode::operateData2D(const ProcessPool &procPool, Configuration *cfg)
{
    // We expect x values to be centre-bin values, and regularly spaced
    const auto &xAxis = targetData2D_->xAxis();
    const auto &yAxis = targetData2D_->yAxis();
    auto &values = targetData2D_->values();
    if (xAxis.size() < 2)
        return true;

    // Derive first left-bin boundary from the delta between points 0 and 1
    double leftBin = xAxis[0] - (xAxis[1] - xAxis[0]) * 0.5, rightBin, divisor;
    double r1Cubed = pow(leftBin, 3), r2Cubed;
    for (auto n = 0; n < xAxis.size(); ++n)
    {
        for (auto m = 0; m < yAxis.size(); ++m)
        {
            // Get new right-bin from existing left bin boundary and current bin centre
            rightBin = leftBin + 2 * (xAxis[n] - leftBin);
            r2Cubed = pow(rightBin, 3);
            divisor = (4.0 / 3.0) * PI * (r2Cubed - r1Cubed);
            values[{n, m}] /= divisor;
            if (targetData2D_->valuesHaveErrors())
                targetData2D_->error(n, m) /= divisor;
        }

        // Overwrite old values
        r1Cubed = r2Cubed;
        leftBin = rightBin;
    }

    return true;
}
