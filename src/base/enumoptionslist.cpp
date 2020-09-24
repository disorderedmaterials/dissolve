// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "base/enumoptionslist.h"

// Return list of options
const std::vector<EnumOption> &EnumOptionsList::options() const { return options_; }

// Add item
EnumOptionsList &EnumOptionsList::operator<<(EnumOption option)
{
    options_.push_back(option);
    return (*this);
}
