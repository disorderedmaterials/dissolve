// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/calculate_cn/cn.h"
#include "procedure/nodes/sum1d.h"

// Return specified coordination number (from Sum1D node)
const SampledDouble &CalculateCNModule::coordinationNumber(int index)
{
    if (!sum1D_)
    {
        static const SampledDouble dummy;
        return dummy;
    }

    return sum1D_->sum(index);
}

// Return whether range B is enabled
bool CalculateCNModule::isRangeBEnabled() const { return (sum1D_ ? sum1D_->isRangeBEnabled() : false); }

// Return whether range C is enabled
bool CalculateCNModule::isRangeCEnabled() const { return (sum1D_ ? sum1D_->isRangeCEnabled() : false); }
