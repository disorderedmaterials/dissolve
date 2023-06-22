// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <toml11/toml.hpp>

#include <map>
#include <vector>

// The type we use for the nodes of our serialisation tree
using SerialisedValue = toml::value;

// An interface for classes that can be serialised into an input file
template <typename... Contexts> class Serialisable
{
    public:
    // Express as a serialisable value
    virtual SerialisedValue serialise() const = 0;
    // Read values from a serialisable value
    virtual void deserialise(const SerialisedValue &node, Contexts... context) { return; }

    /* Functions that hook into the toml11 library */
    // Wrapper for deserialise that toml11 will check for
    void from_toml(const toml::value &node) { deserialise(node); }
    // wrapper for serialise that toml11 will check for
    toml::value into_toml() const { return serialise(); }

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
    static SerialisedValue fromVectorToTable(const std::vector<T> &vector, Lambda getName)
    {
        SerialisedValue group;
        for (auto it = vector.rbegin(); it < vector.rend(); it++)
            group[getName(*it)] = (*it)->serialise();
        return group;
    };
    // A helper function to add elements of a vector to a node under the named heading
    template <typename T, typename Lambda>
    static void fromVectorToTable(const std::vector<T> &vector, std::string name, SerialisedValue &node, Lambda getName)
    {
        if (vector.empty())
            return;
        node[name] = fromVectorToTable(vector, getName);
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
        node[name] = fromVector(vector, toSerial);
    }
    // A helper function to add the elements of a vector to a node under a name
    template <typename T, typename Lambda> static SerialisedValue fromVector(const std::vector<T> &vector, Lambda toSerial)
    {
        SerialisedValue result = toml::array{};
        std::transform(vector.begin(), vector.end(), std::back_inserter(result), toSerial);
        return result;
    }

    // Act over each value in a node table, if the key exists
    template <typename Lambda> static void toMap(const SerialisedValue &node, Lambda action)
    {
        for (auto &[key, value] : node.as_table())
            action(key, value);
    }

    // Act over each value in a node table, if the key exists
    template <typename Lambda> static void toMap(const SerialisedValue &node, std::string key, Lambda action)
    {
        if (node.contains(key))
            for (auto &[key, value] : toml::find<toml::table>(node, key))
                action(key, value);
    }

    // Act over each value in a node array
    template <typename Lambda> static void toVector(const SerialisedValue &node, Lambda action)
    {
      for (auto &item : node.as_array())
	    action(item);
    }

    // Act over each value in a node table, if the key exists
    template <typename Lambda> static void toVector(const SerialisedValue &node, std::string key, Lambda action)
    {
        if (node.contains(key))
	    toVector(node.at(key), action); 
    }
};
