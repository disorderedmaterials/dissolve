// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include <vector>

// Keyed Vector
template <typename KeyClass, typename DataClass> class KeyedVector
{
    public:
    using KeyValuePair = std::pair<KeyClass, DataClass>;

    private:
    // Vector of data
    std::vector<KeyValuePair> data_;

    public:
    DataClass &operator[](KeyClass key)
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first == key; });
        return (it == data_.end()) ? data_.emplace(KeyValuePair(key, DataClass())).second : it->second;
    }
    // Set / overwrite key
    void set(const KeyClass key, DataClass data) { operator[](key) = data; }
    // Add to existing key (or create new)
    void add(const KeyClass key, DataClass data)
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first == key; });
        if (it == data_.end())
            data_.emplace(KeyValuePair(key, data));
        else
            it->second += data;
    }
    // Remove the specified key
    void remove(KeyClass key)
    {
        auto it = std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first == key; });
        if (it != data_.end())
            data_.erase(it);
    }
    // Return whether the specified key exists
    bool contains(KeyClass key) const
    {
        return std::ranges::find_if(data_, [&key](const auto &pair) { return pair.first == key; }) != data_.end();
    }
    // Iterators
    std::vector<KeyValuePair>::const_iterator begin() { return data_.begin(); }
    std::vector<KeyValuePair>::const_iterator begin() const { return data_.begin(); }
    std::vector<KeyValuePair>::const_iterator end() { return data_.end(); }
    std::vector<KeyValuePair>::const_iterator end() const { return data_.end(); }
    // Return number of pairs
    int size() const { return data_.size(); }
    // Return a copy of the vector with values modified by the provided lambda
    std::vector<KeyValuePair> operated(std::function<DataClass(const DataClass &original)> lambda)
    {
        auto result = data_;
        for (auto &[_, value] : result)
            value = lambda(value);
        return result;
    }
};
