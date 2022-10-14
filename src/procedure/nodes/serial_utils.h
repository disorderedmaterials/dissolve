// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "procedure/nodevalue.h"
#include <utility>

template <typename T>
std::pair<T, Units::DensityUnits> find_density(const SerialisedValue &node) {
    auto magnitude = toml::find<T>(node, "magnitude");
    auto unit = Units::densityUnits().deserialise(node.at("unit"));
    return {magnitude, unit};
}

template <typename T>
SerialisedValue write_density(const std::pair<T, Units::DensityUnits> &density) {
    SerialisedValue result;
    result["magnitude"] = density.first;
    result["unit"] = Units::densityUnits().serialise(density.second);
    return result;
}