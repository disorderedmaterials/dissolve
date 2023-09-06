// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "kernels/potentials/types.h"

// External Potential Types
namespace ExternalPotentialTypes
{
// External Potential Types
EnumOptions<ExternalPotentialType> types_("ExternalPotential", {{ExternalPotentialType::Simple, "Simple"},
                                                                {ExternalPotentialType::Directional, "Directional"},
                                                                {ExternalPotentialType::Regional, "Regional"}});

// Return whether the supplied external potential type is valid
std::optional<ExternalPotentialType> isType(std::string_view name)
{
    if (types_.isValid(name))
        return types_.enumeration(name);
    return {};
}

// Return whether the supplied external potential type is valid
std::string keyword(ExternalPotentialType type) { return types_.keyword(type); }
}; // namespace ExternalPotentialTypes
