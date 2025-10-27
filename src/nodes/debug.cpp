// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/debug.h"
#include <format>

namespace GraphDebug
{
// Current indent for printing
int indent_{0};
// Return current indent
std::string indent() { return std::format("{:02d}{}", indent_, std::string(std::max(indent_, 0) * 2, ' ')); }
// Increase current indent
void increaseIndent() { ++indent_; }
// Decrease current indent
void decreaseIndent() { --indent_; }
}; // namespace GraphDebug
