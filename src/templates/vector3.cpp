// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "templates/vector3.h"
#include "procedure/nodeValue.h"

template<>
void Vec3<double>::deserialise(const SerialisedValue &node) {
	x = toml::get<double>(node[0]);
	y = toml::get<double>(node[1]);
	z = toml::get<double>(node[2]);
}

template<>
void Vec3<float>::deserialise(const SerialisedValue &node) {
	x = toml::get<float>(node[0]);
	y = toml::get<float>(node[1]);
	z = toml::get<float>(node[2]);
}

template<>
void Vec3<int>::deserialise(const SerialisedValue &node) {
	x = toml::get<int>(node[0]);
	y = toml::get<int>(node[1]);
	z = toml::get<int>(node[2]);
}

template<>
void Vec3<bool>::deserialise(const SerialisedValue &node) {
	x = toml::get<bool>(node[0]);
	y = toml::get<bool>(node[1]);
	z = toml::get<bool>(node[2]);
}

template<>
void Vec3<NodeValue>::deserialise(const SerialisedValue &node)
{
    throw std::runtime_error("Cannot build NodeValue witout context");
}
