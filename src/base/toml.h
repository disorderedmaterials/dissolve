// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include <toml11/toml.hpp>

#include <map>
#include <vector>

// The type we use for the nodes of our serialisation tree
using TomlTable = toml::basic_value<toml::discard_comments, std::map, std::vector>;

// An interface for classes that can be serialised into an input file
class Serialisable
{
    public:
    // Express as a tree node
    virtual TomlTable serialise() const = 0;
    // Read values from a tree node
    virtual void deserialise(TomlTable node) { return; }
    // A helper function to add elements of a vector to a node under the named heading
    template <typename T>
    static void fromVectorToTable(const std::vector<std::shared_ptr<T>> &vector, std::string name, TomlTable &node)
    {
	fromVectorToTable(vector, name, node, [](const auto &item) { return item->name().data(); });
    }
    // A helper function to add elements of a vector to a node under the named heading
    template <typename T>
    static void fromVectorToTable(const std::vector<std::unique_ptr<T>> &vector, std::string name, TomlTable &node)
    {
	fromVectorToTable(vector, name, node, [](const auto &item) { return item->name().data(); });
    }
    // A helper function to add elements of a vector to a node under the named heading
    template <typename T, typename Lambda>
    static void fromVectorToTable(const std::vector<T> &vector, std::string name, TomlTable &node, Lambda getName)
    {
	if (vector.empty())
	    return;
	TomlTable group;
	for (auto &item : vector)
	    group[getName(item)] = item->serialise();
	node[name] = group;
    };
    // A helper function to add the elements of a vector to a node under a name
    template <typename T>
    static void fromVector(const std::vector<std::unique_ptr<T>> &vector, std::string name, TomlTable &node)
    {
	fromVector(vector, name, node, [](const auto &item) { return item->serialise(); });
    }
    // A helper function to add the elements of a vector to a node under a name
    template <typename T> static void fromVector(const std::vector<T> &vector, std::string name, TomlTable &node)
    {
	fromVector(vector, name, node, [](const auto &item) { return item.serialise(); });
    }
    // A helper function to add the elements of a vector to a node under a name
    template <typename T, typename Lambda>
    static void fromVector(const std::vector<T> &vector, std::string name, TomlTable &node, Lambda toSerial)
    {
	if (vector.empty())
	    return;
	toml::array result;
	for (auto &item : vector)
	    result.push_back(toSerial(item));
	node[name] = result;
    }
};
