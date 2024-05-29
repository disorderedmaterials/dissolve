// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <vector>

// Forward Declarations
class Data1D;

// Derivative
namespace Derivative
{
// Return derivative of supplied Data1D
Data1D derivative(const Data1D &source);
}; // namespace Derivative
