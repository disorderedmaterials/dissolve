// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "templates/optionalRef.h"
#include "templates/resolvable.h"
#include <functional>
#include <vector>

// Keyed Vector
template <class KeyClass, class ValueClass> class KeyedVector
{
    public:
    using KeyValuePair = std::pair<KeyClass, ValueClass>;

    private:
    // Vector of data
    std::vector<KeyValuePair> data_;

    public:
    // Clear data
    void clear() { data_.clear(); }
    // Set / overwrite key
    void set(KeyClass key, ValueClass value) { operator[](key) = value; }
    // Add to existing key (or create new)
    void add(KeyClass key, ValueClass value)
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first == key; });
        if (it == data_.end())
            data_.emplace_back(key, value);
        else
            it->second += value;
    }
    // Merge keys / values from the supplied KeyedVector, replacing any existing values
    void merge(const KeyedVector<KeyClass, ValueClass> &other)
    {
        for (auto &[key, value] : other)
            operator[](key) = value;
    }
    // Remove the specified key
    void erase(KeyClass key)
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first == key; });
        if (it != data_.end())
            data_.erase(it);
    }
    // Remove matching keys
    void erase(std::function<bool(const KeyClass &key)> lambda)
    {
        data_.erase(std::remove_if(data_.begin(), data_.end(), [lambda](const auto &pair) { return lambda(pair.first); }),
                    data_.end());
    }
    // Return whether the specified key exists
    bool contains(KeyClass key) const
    {
        return std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first == key; }) != data_.end();
    }
    // Change key for another, overwriting the destination key if it already exists
    void changeKey(KeyClass fromKey, KeyClass toKey)
    {
        auto fromIt = std::ranges::find_if(data_, [&fromKey](const auto &pair) { return pair.first == fromKey; });
        if (fromIt == data_.end())
            return;

        auto toIt = std::ranges::find_if(data_, [&toKey](const auto &pair) { return pair.first == toKey; });
        if (toIt == data_.end())
        {
            // Just replace the existing key
            fromIt->first = toKey;
        }
        else
        {
            // Set the toKey value to the fromKey value, then remove fromKey
            toIt->second = fromIt->second;
            erase(fromKey);
        }
    }
    // Element access operator []
    ValueClass &operator[](KeyClass key)
        requires(std::is_default_constructible_v<ValueClass> &&
                 !(std::is_integral_v<ValueClass> || std::is_floating_point_v<ValueClass>))
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first == key; });
        return (it == data_.end()) ? data_.emplace_back(key, ValueClass()).second : it->second;
    }
    ValueClass &operator[](KeyClass key)
        requires(std::is_integral_v<ValueClass> || std::is_floating_point_v<ValueClass>)
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first == key; });
        return (it == data_.end()) ? data_.emplace_back(key, 0).second : it->second;
    }
    // Get keyed value
    const ValueClass &value(KeyClass key) const
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first == key; });
        if (it == data_.end())
            throw(std::runtime_error(std::format("Key not found in KeyedVector.\n")));

        return it->second;
    }
    // Get keyed value or return default if it doesn't exist
    ValueClass valueOr(KeyClass key, ValueClass defaultValue) const
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first == key; });
        if (it == data_.end())
            return defaultValue;

        return it->second;
    }
    // Indexed access
    KeyValuePair &pair(int index) { return data_[index]; }
    const KeyValuePair &pair(int index) const { return data_[index]; }
    KeyClass &key(int index) { return data_[index].first; }
    const KeyClass &key(int index) const { return data_[index].first; }
    ValueClass &value(int index) { return data_[index].second; }
    const ValueClass &value(int index) const { return data_[index].second; }
    // Iterators
    std::vector<KeyValuePair>::iterator begin() { return data_.begin(); }
    std::vector<KeyValuePair>::const_iterator begin() const { return data_.begin(); }
    std::vector<KeyValuePair>::iterator end() { return data_.end(); }
    std::vector<KeyValuePair>::const_iterator end() const { return data_.end(); }
    // Return the underlying vector
    std::vector<KeyValuePair> &vector() { return data_; }
    const std::vector<KeyValuePair> &vector() const { return data_; }
    // Return number of pairs
    int size() const { return data_.size(); }
    // Return a copy of the vector with values modified by the provided lambda
    KeyedVector<KeyClass, ValueClass> operated(std::function<ValueClass(const ValueClass &original)> lambda) const
    {
        KeyedVector<KeyClass, ValueClass> result;
        for (auto &[key, value] : data_)
            result.set(key, lambda(value));
        return result;
    }
};

// Resolvable Keyed Vector
template <class KeyClass, class ValueClass> class ResolvableKeyedVector
{
    public:
    using KeyValuePair = std::pair<Resolvable<KeyClass>, ValueClass>;

    private:
    // Vector of data
    std::vector<KeyValuePair> data_;

    public:
    // Clear data
    void clear() { data_.clear(); }
    // Set / overwrite key
    void set(KeyClass key, ValueClass value) { operator[](key) = value; }
    // Add to existing key (or create new)
    void add(KeyClass key, ValueClass value)
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first == key; });
        if (it == data_.end())
            data_.emplace_back(key, value);
        else
            it->second += value;
    }
    // Merge keys / values from the supplied KeyedVector, replacing any existing values
    void merge(const ResolvableKeyedVector<KeyClass, ValueClass> &other)
    {
        for (auto &[key, value] : other)
            operator[](key) = value;
    }
    // Remove the specified key
    void erase(KeyClass key)
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first.raw() == key; });
        if (it != data_.end())
            data_.erase(it);
    }
    // Remove matching keys
    void erase(std::function<bool(const KeyClass &key)> lambda)
    {
        data_.erase(std::remove_if(data_.begin(), data_.end(), [lambda](const auto &pair) { return lambda(pair.first.raw()); }),
                    data_.end());
    }
    // Return whether the specified key exists
    bool contains(KeyClass key) const
    {
        return std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first.raw() == key; }) != data_.end();
    }
    // Change key for another, overwriting the destination key if it already exists
    void changeKey(KeyClass fromKey, KeyClass toKey)
    {
        auto fromIt = std::ranges::find_if(data_, [&fromKey](const auto &pair) { return pair.first.raw() == fromKey; });
        if (fromIt == data_.end())
            return;

        auto toIt = std::ranges::find_if(data_, [&toKey](const auto &pair) { return pair.first.raw() == toKey; });
        if (toIt == data_.end())
        {
            // Just replace the existing key
            fromIt->first = toKey;
        }
        else
        {
            // Set the toKey value to the fromKey value, then remove fromKey
            toIt->second = fromIt->second;
            erase(fromKey);
        }
    }
    // Element access operator []
    ValueClass &operator[](KeyClass key)
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first.raw() == key; });
        return (it == data_.end()) ? data_.emplace_back(Resolvable(key), ValueClass()).second : it->second;
    }
    ValueClass &operator[](std::string_view resolvedName)
    {
        auto it = std::ranges::find_if(data_, [&resolvedName](const auto &pair) { return pair.first.name() == resolvedName; });
        return (it == data_.end()) ? data_.emplace_back(Resolvable<KeyClass>(resolvedName), ValueClass()).second : it->second;
    }
    // Get keyed value
    const ValueClass &value(KeyClass key) const
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first.raw() == key; });
        if (it == data_.end())
            throw(std::runtime_error(std::format("Key not found in KeyedVector.\n")));

        return it->second;
    }
    // Get keyed value or return default if it doesn't exist
    ValueClass valueOr(KeyClass key, ValueClass defaultValue) const
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first.raw() == key; });
        if (it == data_.end())
            return defaultValue;

        return it->second;
    }
    // Resolve named item to have the supplied key
    void resolve(std::string_view name, KeyClass key)
    {
        auto it = std::ranges::find_if(data_, [&name](const auto &pair) { return pair.first.name() == name; });
        if (it == data_.end())
            throw(std::runtime_error(std::format("Named resolvable '{}' not found in KeyedVector.\n", name)));
        it->first.resolve(key);
    }
    // Indexed access
    KeyValuePair &pair(int index) { return data_[index]; }
    const KeyValuePair &pair(int index) const { return data_[index]; }
    KeyClass &key(int index) { return data_[index].first.raw(); }
    const KeyClass &key(int index) const { return data_[index].first.raw(); }
    ValueClass &value(int index) { return data_[index].second; }
    const ValueClass &value(int index) const { return data_[index].second; }
    // Iterators
    std::vector<KeyValuePair>::iterator begin() { return data_.begin(); }
    std::vector<KeyValuePair>::const_iterator begin() const { return data_.begin(); }
    std::vector<KeyValuePair>::iterator end() { return data_.end(); }
    std::vector<KeyValuePair>::const_iterator end() const { return data_.end(); }
    // Return the underlying vector
    std::vector<KeyValuePair> &vector() { return data_; }
    const std::vector<KeyValuePair> &vector() const { return data_; }
    // Return number of pairs
    int size() const { return data_.size(); }
    // Return a copy of the vector with values modified by the provided lambda
    KeyedVector<KeyClass, ValueClass> operated(std::function<ValueClass(const ValueClass &original)> lambda) const
    {
        KeyedVector<KeyClass, ValueClass> result;
        for (auto &[key, value] : data_)
            result.set(key, lambda(value));
        return result;
    }
    // Convert to standard KeyedVector
    operator KeyedVector<KeyClass, ValueClass>() const
    {
        KeyedVector<KeyClass, ValueClass> result;
        for (auto &[resolvable, value] : data_)
            result[resolvable.raw()] = value;
        return result;
    }
};
