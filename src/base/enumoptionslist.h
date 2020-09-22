// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/enumoption.h"
#include <vector>

// Enum Options List
class EnumOptionsList
{
    private:
    // List of options
    std::vector<EnumOption> options_;

    public:
    // Return list of options
    const std::vector<EnumOption> &options() const;
    // Add item
    EnumOptionsList &operator<<(EnumOption option);
};
