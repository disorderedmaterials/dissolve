// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/operatesphericalshellnormalise.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/types.h"
#include "procedure/nodes/select.h"

OperateSphericalShellNormaliseProcedureNode::OperateSphericalShellNormaliseProcedureNode()
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateSphericalShellNormalise)
{
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateSphericalShellNormaliseProcedureNode::operateData1D(ProcessPool &procPool, Configuration *cfg)
{
    // We expect x values to be centre-bin values, and regularly spaced
    const auto &xAxis = targetData1D_->xAxis();
    auto &values = targetData1D_->values();
    if (xAxis.size() < 2)
        return true;

    // Derive first left-bin boundary from the delta betwen points 0 and 1
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
