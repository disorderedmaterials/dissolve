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
  if (!inputs_.contains(name))
    return {};
  return inputs_.at(name);
}

/*
 * Inputs
 */

// Return input parameters
std::map<std::string_view, std::shared_ptr<ParameterBase>> &Node::inputs() { return inputs_; };
