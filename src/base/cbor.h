// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <iostream>
#include <ranges>
#include <tuple>
#include <vector>

namespace CBOR
{
// Convert a serialed Value to its CBOR representation
std::vector<uint8_t> to(const SerialisedValue &node);

// // Parse a CBOR representation of a serialised value
SerialisedValue from(std::ranges::subrange<std::vector<uint8_t>::iterator> bytes);

SerialisedValue from(std::ifstream infile);

} // namespace CBOR
