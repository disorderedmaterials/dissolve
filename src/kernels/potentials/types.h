// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include <optional>

// External Potential Types
namespace ExternalPotentialTypes
{
// External Potential Types
enum class ExternalPotentialType
{
    Spherical,
    Cylindrical,
    Regional
};
// Return whether the supplied external potential type is valid
std::optional<ExternalPotentialType> isType(std::string_view name);
// Return whether the supplied external potential type is valid
std::string keyword(ExternalPotentialType type);
}; // namespace ExternalPotentialTypes
