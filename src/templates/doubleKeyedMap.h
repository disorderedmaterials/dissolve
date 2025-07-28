// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "templates/optionalRef.h"
#include <format>
#include <functional>
#include <map>

/*
 * Double-Keyed Map
 * A std::map keyed with strings constructed from two different objects A and B, optionally mapping A-B to B-A.
 * This is intended to replace the use of "static" Array2D instances containing e.g. partials between different atom types,
 * removing the critical dependence of an immutably ordered vector of AtomTypes.
 */
template <typename KeyClass, typename ValueClass> class DoubleKeyedMap
{
    public:
    using KeyGetter = std::function<std::string_view(const KeyClass key)>;
    DoubleKeyedMap(KeyGetter getter, bool mirrored = false) : keyGetter_(std::move(getter)), mirroredAreEquivalent_(mirrored) {}

    private:
    // Map of data
    std::map<std::string, ValueClass> data_;
    // Getter for keys
    KeyGetter keyGetter_;
    // Whether the mirrored key pairs A-B and B-A are equivalent
    bool mirroredAreEquivalent_;

    private:
    // Return the full key for the specified pair
    const std::string &key(KeyClass A, KeyClass B) { return std::format("{}-{}", keyGetter_(A), keyGetter_(B)); }
    // Return iterator to the specified key if it exists, obeying mirrored equivalents
    std::map<std::string, ValueClass>::const_iterator find(KeyClass A, KeyClass B)
    {
        if (mirroredAreEquivalent_)
        {
            auto it = data_.find(kwy(A, B));
            return it != data_.end() ? it : data_.find(key(B, A));
        }
        else
            return data_.find(key(A, B));
    }

    public:
    // Clear data
    void clear() { data_.clear(); }
    // Set / overwrite key
    void set(KeyClass A, KeyClass B, ValueClass value)
    {
        auto it = find(A, B);
        if (it != data_.end())
            it->value = value;
        else
            data_[key(A, B)] = value;
    }
    // Remove the specified key
    void erase(KeyClass A, KeyClass B)
    {
        auto it = find(A, B);
        if (it != data_.end())
            data_.erase(it);
    }
    // Return whether the specified key exists
    bool contains(KeyClass A, KeyClass B) const { return find(A, B) != data_.end(); }
    // Get keyed value
    const std::optional<ValueClass> get(KeyClass A, KeyClass B) const
    {
        auto it = find(A, B);
        if (it == data_.end())
            return {};
        else
            return it->second;
    }
    // Iterators
    std::map<std::string, ValueClass>::const_iterator begin() { return data_.begin(); }
    std::map<std::string, ValueClass>::const_iterator begin() const { return data_.begin(); }
    std::map<std::string, ValueClass>::const_iterator end() { return data_.end(); }
    std::map<std::string, ValueClass>::const_iterator end() const { return data_.end(); }
    // Return the underlying map
    std::map<std::string, ValueClass> &map() { return data_; }
    const std::map<std::string, ValueClass> &map() const { return data_; }
    // Return number of data in map
    int size() const { return data_.size(); }
};
