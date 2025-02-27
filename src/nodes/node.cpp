// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/node.h"
#include "base/sysFunc.h"

/*
 * Inputs
 */

// Return named input parameter if it exists
std::shared_ptr<ParameterBase> Node::findInput(std::string_view name) const
{
    auto it = std::find_if(inputs_.begin(), inputs_.end(),
                           [name](const auto &p) { return DissolveSys::sameString(p->name(), name); });
    if (it == inputs_.end())
        return {};
    else
        return *it;
}

/*
 * Inputs
 */

// Return input parameters
std::vector<std::shared_ptr<ParameterBase>> &Node::inputs() { return inputs_; };
