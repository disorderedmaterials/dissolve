// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "templates/orderedMap.h"
#include <toml11/toml.hpp>
#include <vector>

// The type we use for the nodes of our serialisation tree
using SerialisedValue = toml::basic_value<toml::discard_comments, dissolve::OrderedMap, std::vector>;

namespace Serialisable
{

void serialiseOnto(const int a, std::string tag, SerialisedValue &target);
void serialiseOnto(const double a, std::string tag, SerialisedValue &target);
void serialiseOnto(const std::string a, std::string tag, SerialisedValue &target);
} // namespace Serialisable

namespace Deserialisable
{

void deserialiseOnto(bool &a, const SerialisedValue &target);
void deserialiseOnto(int &a, const SerialisedValue &target);
void deserialiseOnto(long &a, const SerialisedValue &target);
void deserialiseOnto(float &a, const SerialisedValue &target);
void deserialiseOnto(double &a, const SerialisedValue &target);
void deserialiseOnto(std::string &a, const SerialisedValue &target);
} // namespace Deserialisable
