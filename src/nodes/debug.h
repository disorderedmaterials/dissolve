// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once
#include <string>

namespace GraphDebug
{
// Return current indent
std::string indent();
// Increase current indent
void increaseIndent();
// Decrease current indent
void decreaseIndent();
}; // namespace GraphDebug
