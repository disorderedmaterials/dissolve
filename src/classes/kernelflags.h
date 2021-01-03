// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

// Forward Declarations
/* none */

// Kernel Flags
class KernelFlags
{
    public:
    // Flags controlling Energy and ForceKernel routines. Note that at most, only one Exclude-type flag and the
    // ApplyMinimumImage flag should be or'd together
    enum Flags
    {
        NoFlags = 0,
        ExcludeSelfFlag = 1,
        ExcludeIGEJFlag = 2,
        ExcludeIntraIGEJFlag = 4,
        ApplyMinimumImageFlag = 8
    };
};
