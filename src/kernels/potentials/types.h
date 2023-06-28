// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include <optional>

// External Potential Types
namespace ExternalPotentialTypes
{
// External Potential Types
enum class ExternalPotentialType
{
    Simple,
    Directional
};
// Return whether the supplied external potential type is valid
std::optional<ExternalPotentialType> isType(std::string_view name);
// Return whether the supplied external potential type is valid
std::string keyword(ExternalPotentialType type);
}; // namespace ExternalPotentialTypes
