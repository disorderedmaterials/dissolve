// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "base/sysFunc.h"
#include "templates/algorithms.h"
#include "templates/array2D.h"
#include <format>
#include <map>

/*
 * Double-Keyed Map
 * A std::map keyed with strings constructed from two different objects A and B, optionally mapping A-B to B-A.
 * This is intended to replace the use of "static" Array2D instances containing e.g. partials between different atom types,
 * removing the critical dependence of an immutably ordered vector of AtomTypes.
 */
using DoubleKeyedMapKey = std::pair<std::string_view, std::string_view>;
template <typename ValueClass> class DoubleKeyedMap
{
    public:
    DoubleKeyedMap(bool mirrored = false) : mirroredAreEquivalent_(mirrored) {}

    private:
    // Separator to use in key creation
    std::string separator_{"//"};
    // Map of data
    std::map<std::string, ValueClass> data_;
    // Whether the mirrored key pairs A-B and B-A are equivalent
    bool mirroredAreEquivalent_;

    private:
    // Return the full key for the specified pair
    const std::string key(std::string_view A, std::string_view B) const { return std::format("{}{}{}", A, separator_, B); }
    // Return iterator to the specified key if it exists, obeying mirrored equivalents
    std::map<std::string, ValueClass>::iterator find(std::string_view A, std::string_view B)
    {
        if (mirroredAreEquivalent_)
        {
            auto it = data_.find(key(A, B));
            return it != data_.end() ? it : data_.find(key(B, A));
        }
        else
            return data_.find(key(A, B));
    }
    std::map<std::string, ValueClass>::const_iterator find(std::string_view A, std::string_view B) const
    {
        if (mirroredAreEquivalent_)
        {
            auto it = data_.find(key(A, B));
            return it != data_.end() ? it : data_.find(key(B, A));
        }
        else
            return data_.find(key(A, B));
    }
    std::map<std::string, ValueClass>::const_iterator find(const std::pair<std::string_view, std::string_view> &pair) const
    {
        return find(pair.first, pair.second);
    }

    public:
    // Clear data
    void clear() { data_.clear(); }
    // Return whether the mirrored key pairs A-B and B-A are equivalent
    bool mirroredAreEquivalent() const { return mirroredAreEquivalent_; }
    // Set / overwrite key
    void set(const std::pair<std::string_view, std::string_view> &pair, const ValueClass &value)
    {
        set(pair.first, pair.second, value);
    }
    void set(std::string_view A, std::string_view B, const ValueClass &value)
    {
        auto it = find(A, B);
        if (it != data_.end())
            it->second = value;
        else
            data_[key(A, B)] = value;
    }
    // Remove the specified key
    void erase(std::string_view A, std::string_view B)
    {
        auto it = find(A, B);
        if (it != data_.end())
            data_.erase(it);
    }
    // Return key parts from supplied string
    std::pair<std::string, std::string> keyPair(std::string_view key) const
    {
        auto keys = DissolveSys::splitString(key, separator_);
        if (keys.size() != 2)
            throw(std::runtime_error(std::format("DoubleKeyedMap - can't split supplied key '{}' into a key pair.\n", key)));
        return {std::string(keys[0]), std::string(keys[1])};
    }
    // Return whether the specified key exists
    bool contains(const std::pair<std::string_view, std::string_view> &pair) const { return contains(pair.first, pair.second); }
    bool contains(std::string_view A, std::string_view B) const { return find(A, B) != data_.end(); }
    bool contains(std::string_view key) const { return find(keyPair(key)) != data_.end(); }
    // Get keyed value
    ValueClass &operator[](const std::string_view key) { return get(key); }
    const ValueClass &operator[](const std::string_view key) const { return get(key); }
    ValueClass &operator[](const std::pair<std::string_view, std::string_view> &pair) { return get(pair.first, pair.second); }
    const ValueClass &operator[](const std::pair<std::string_view, std::string_view> &pair) const
    {
        return get(pair.first, pair.second);
    }
    ValueClass &get(const std::string_view key) { return get(keyPair(key)); }
    const ValueClass &get(const std::string_view key) const { return get(keyPair(key)); }
    ValueClass &get(const std::pair<std::string_view, std::string_view> &pair) { return get(pair.first, pair.second); }
    const ValueClass &get(const std::pair<std::string_view, std::string_view> &pair) const
    {
        return get(pair.first, pair.second);
    }
    ValueClass &get(std::string_view A, std::string_view B)
    {
        auto it = find(A, B);
        if (it == data_.end())
        {
            data_[key(A, B)] = ValueClass();
            return data_[key(A, B)];
        }
        else
            return it->second;
    }

    const ValueClass &get(std::string_view A, std::string_view B) const
    {
        auto it = find(A, B);
        if (it == data_.end())
            throw(std::runtime_error(std::format("Key {} is not in DoubleKeyedMap.\n", key(A, B))));
        else
            return it->second;
    }
    // Iterators
    std::map<std::string, ValueClass>::iterator begin() { return data_.begin(); }
    std::map<std::string, ValueClass>::const_iterator begin() const { return data_.begin(); }
    std::map<std::string, ValueClass>::iterator end() { return data_.end(); }
    std::map<std::string, ValueClass>::const_iterator end() const { return data_.end(); }
    // Return the underlying map
    std::map<std::string, ValueClass> &map() { return data_; }
    const std::map<std::string, ValueClass> &map() const { return data_; }
    // Return number of data in map
    int size() const { return data_.size(); }

    /*
     * Look-Up Table
     */
    public:
    // Return look-up table mapping element indices in the supplied vector to corresponding map entries
    template <std::ranges::range Range, class Lam>
    Array2D<typename std::map<std::string, ValueClass>::iterator> lookUpTable(Range keyedObjects, Lam keyGetter)
    {
        Array2D<typename std::map<std::string, ValueClass>::iterator> result;
        auto nElements = keyedObjects.size();
        result.initialise(nElements, nElements, mirroredAreEquivalent_);
        dissolve::for_each_pair(
            ParallelPolicies::seq, keyedObjects, [&](int i, const auto &itemI, int j, const auto &itemJ)
            { result[{i, j}] = find(keyGetter(itemI), keyGetter(itemJ)); }, mirroredAreEquivalent_);
        return result;
    }

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const
    {
        SerialisedValue result;
        Serialisable::fromMap(data_, "map", result);
        target[tag] = result;
    };
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node)
    {
        data_.clear();

        for (auto &[mapKey, value] : toml::find<SerialisedValue::table_type>(node, "map"))
        {
            if constexpr (std::is_same_v<ValueClass, double>)
                data_[mapKey] = value.as_floating();
            else if constexpr (std::is_same_v<ValueClass, int>)
                data_[mapKey] = value.as_integer();
            else if constexpr (std::is_same_v<ValueClass, bool>)
                data_[mapKey] = value.as_boolean();
            else
                data_[mapKey].deserialise(value);
        }
    }
};
