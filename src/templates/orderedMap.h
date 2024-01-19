// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <algorithm>
#include <map>
#include <vector>

namespace dissolve
{

// This class is similar to std::map, but the values are returned in
// order of insertion, rather than in alphabetical order (like
// std::map) or random order (like std:unordered_map)
//
// Access is still O(1) and insertion is O(log(N)).  Erasure hasn't
// been implemented yet and will likely be O(N), unlike the O(log(N))
// of std::map.  However, seeing as removal hasn't even been required
// at this point, it seems unlikely that it will ever serve as a true
// bottleneck.
template <typename Key, typename Value> class OrderedMap
{

    private:
    // A mapping between keys and indices
    std::map<Key, int> index_;
    // An association array of the keys and values
    std::vector<std::pair<Key, Value>> values_;
    // If a key does not exist, add it to the array
    std::pair<Key, Value> &insistKey(const Key &key)
    {
        if (index_.find(key) == index_.end())
        {
            index_[key] = values_.size();
            return values_.emplace_back(key, Value{});
        }
        else
            return values_[index_[key]];
    }

    public:
    // Constructors
    OrderedMap(){};
    // Constructor from an iterator off a std::map or std::unorderedmap
    template <typename Iter> OrderedMap(Iter begin, Iter end)
    {
        for (auto i = begin; i != end; i++)
            (*this)[i->first] = i->second;
    };

    // Iterator access
    typename std::vector<std::pair<Key, Value>>::iterator begin() { return values_.begin(); }
    typename std::vector<std::pair<Key, Value>>::iterator end() { return values_.end(); }
    typename std::vector<std::pair<Key, Value>>::const_iterator begin() const { return values_.begin(); }
    typename std::vector<std::pair<Key, Value>>::const_iterator end() const { return values_.end(); }

    // Required std::map interface functions
    const Value &at(const Key &key) const { return values_[index_.at(key)].second; }
    Value &at(const Key &key) { return values_[index_.at(key)].second; }
    // The number of times a key is used.  This can only be zero or one.
    auto count(const Key &key) const { return index_.count(key); }

    // Operator overloads from std::map
    bool operator==(const OrderedMap<Key, Value> other) const
    {
        if (values_.size() != other.values_.size())
            return false;
        for (int i = 0; i < values_.size(); i++)
            if (values_[i] != other.values_[i])
                return false;
        for (auto [k, v] : index_)
            if (other.index_.at(k) != v)
                return false;
        return true;
    }
    Value &operator[](const Key &key) { return insistKey(key).second; }
    Value &operator[](Key &&key) { return insistKey(key).second; }
};

} // namespace dissolve
