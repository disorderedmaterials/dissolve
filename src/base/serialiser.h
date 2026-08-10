// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "templates/orderedMap.h"
#include <toml11/error_info.hpp>
#include <toml11/find.hpp>
#include <toml11/parser.hpp>
#include <toml11/result.hpp>
#include <toml11/source_location.hpp>
#include <vector>

struct wo_comment_config
{
    using comment_type = toml::discard_comments;

    using boolean_type = bool;
    using integer_type = std::int64_t;
    using floating_type = double;
    using string_type = std::string;

    template <typename T> using array_type = std::vector<T>;
    template <typename K, typename T> using table_type = dissolve::OrderedMap<K, T>;

    static toml::result<integer_type, toml::error_info> parse_int(const std::string &str, const toml::source_location src,
                                                                  const std::uint8_t base)
    {
        return toml::read_int<integer_type>(str, src, base);
    }

    static toml::result<floating_type, toml::error_info> parse_float(const std::string &str, const toml::source_location src,
                                                                     const bool is_hex)
    {
        return toml::read_float<floating_type>(str, src, is_hex);
    }
};

// The type we use for the nodes of our serialisation tree
using SerialisedValue = toml::basic_value<wo_comment_config>;

namespace Serialisable
{

using array = toml::array;
using table = toml::table;

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
