// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include <toml11/toml.hpp>

#include <map>
#include <vector>

using TomlTable = toml::basic_value<toml::discard_comments, std::map, std::vector>;

class Serialisable
{
    public:
    virtual TomlTable serialise() const = 0;
    void deserialise(TomlTable node) { return; }
};
