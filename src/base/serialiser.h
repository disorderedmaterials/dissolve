// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <toml11/toml.hpp>

#include <map>
#include <vector>

// The type we use for the nodes of our serialisation tree
using SerialisedValue = toml::value;

// An interface for classes that can be serialised into an input file
class Serialisable
{
    public:
    // Express as a tree node
    virtual SerialisedValue serialise() const = 0;
    // Read values from a tree node
    virtual void deserialise(SerialisedValue &node) { return; }
    // A helper function to add elements of a vector to a node under the named heading
    template <typename T>
    static void fromVectorToTable(const std::vector<std::shared_ptr<T>> &vector, std::string name, SerialisedValue &node)
    {
        fromVectorToTable(vector, name, node, [](const auto &item) { return item->name().data(); });
    }
    // A helper function to add elements of a vector to a node under the named heading
    template <typename T>
    static void fromVectorToTable(const std::vector<std::unique_ptr<T>> &vector, std::string name, SerialisedValue &node)
    {
        fromVectorToTable(vector, name, node, [](const auto &item) { return item->name().data(); });
    }
    // A helper function to add elements of a vector to a node under the named heading
    template <typename T, typename Lambda>
    static void fromVectorToTable(const std::vector<T> &vector, std::string name, SerialisedValue &node, Lambda getName)
    {
        if (vector.empty())
            return;
        SerialisedValue group;
        for (auto &item : vector)
            group[getName(item)] = item->serialise();
        node[name] = group;
    };
    // A helper function to add the elements of a vector to a node under a name
    template <typename T>
    static void fromVector(const std::vector<std::unique_ptr<T>> &vector, std::string name, SerialisedValue &node)
    {
        fromVector(vector, name, node, [](const auto &item) { return item->serialise(); });
    }
    // A helper function to add the elements of a vector to a node under a name
    template <typename T>
    static void fromVector(const std::vector<std::shared_ptr<T>> &vector, std::string name, SerialisedValue &node)
    {
        fromVector(vector, name, node, [](const auto &item) { return item->serialise(); });
    }
    // A helper function to add the elements of a vector to a node under a name
    template <typename T> static void fromVector(const std::vector<T> &vector, std::string name, SerialisedValue &node)
    {
        fromVector(vector, name, node, [](const auto &item) { return item.serialise(); });
    }
    // A helper function to add the elements of a vector to a node under a name
    template <typename T, typename Lambda>
    static void fromVector(const std::vector<T> &vector, std::string name, SerialisedValue &node, Lambda toSerial)
    {
        if (vector.empty())
            return;
        toml::array result;
        for (auto &item : vector)
            result.push_back(toSerial(item));
        node[name] = result;
    }

    // Act over each value in a node table, if the key exists
    template <typename Lambda> static void toMap(SerialisedValue &node, std::string key, Lambda action)
    {
        if (node.contains(key))
            for (auto &[key, value] : node[key].as_table())
                action(key, value);
    }

    // Act over each value in a node table, if the key exists
    template <typename Lambda> static void toVector(SerialisedValue &node, std::string key, Lambda action)
    {
        if (node.contains(key))
            for (auto &item : node[key].as_array())
                action(item);
    }
};