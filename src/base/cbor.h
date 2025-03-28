// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <ranges>
#include <tuple>
#include <vector>

// Convert a serialed Value to its CBOR representation
std::vector<char> toCBOR(const SerialisedValue &node);

// Parse a CBOR representation of a serialised value
std::tuple<SerialisedValue, std::ranges::subrange<std::vector<char>::iterator>>
fromCBOR(std::ranges::subrange<std::vector<char>::iterator> bytes);
