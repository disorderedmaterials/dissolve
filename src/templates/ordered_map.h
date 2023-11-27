// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <algorithm>
#include <map>
#include <vector>

namespace dissolve
{

template <typename Key, typename Value> class ordered_map
{

    private:
    std::map<Key, int> index_;
    std::vector<std::pair<Key, Value>> values_;
    void insistKey_(const Key &key)
    {
        if (index_.find(key) == index_.end())
        {
            index_[key] = values_.size();
            values_.emplace_back(key, Value{});
        }
    }

    public:
    ordered_map(){};
    template <typename Iter> ordered_map(Iter begin, Iter end)
    {
        for (auto i = begin; i != end; i++)
            (*this)[i->first] = i->second;
    };
    typename std::vector<std::pair<Key, Value>>::iterator begin() { return values_.begin(); }
    typename std::vector<std::pair<Key, Value>>::iterator end() { return values_.end(); }
    typename std::vector<std::pair<Key, Value>>::const_iterator begin() const { return values_.begin(); }
    typename std::vector<std::pair<Key, Value>>::const_iterator end() const { return values_.end(); }
    bool operator==(const ordered_map<Key, Value> other) const
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
    auto count(const Key &key) const { return index_.count(key); }
    const Value &at(const Key &key) const { return values_[index_.at(key)].second; }
    Value &at(const Key &key) { return values_[index_.at(key)].second; }
    Value &operator[](const Key &key)
    {
        insistKey_(key);
        return values_[index_[key]].second;
    }
    Value &operator[](Key &&key)
    {
        insistKey_(key);
        return values_[index_[key]].second;
    }
};

} // namespace dissolve
