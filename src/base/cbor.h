// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <iostream>
#include <ranges>
#include <tuple>
#include <variant>
#include <vector>

namespace CBOR
{

// Convert a serialised value to its CBOR representation
std::vector<uint8_t> to(const SerialisedValue &node);

// Parse a CBOR representation of a serialised value
SerialisedValue from(std::ranges::subrange<std::vector<uint8_t>::iterator> bytes);

SerialisedValue from(std::ifstream &&infile);

using PathStep = std::variant<std::string, int>;
using Path = std::vector<PathStep>;

// Pull a piece of a TOML value from a large CBOR file
SerialisedValue extract(std::ifstream &&infile, Path path);

} // namespace CBOR
