// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

namespace DissolveSignals
{
// Data Mutation Flags
enum DataMutations
{
    ConfigurationsMutated,
    IsotopologuesMutated,
    ModulesMutated,
    SpeciesMutated,
    SpeciesSiteMutated
};
}; // namespace DissolveSignals
