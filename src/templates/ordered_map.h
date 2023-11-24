// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <algorithm>
#include <map>
#include <vector>

namespace dissolve
{

template <typename Key, typename Value> class OrderedMapIterator
{
    private:
    std::map<Key, Value> &values_;
    std::vector<Key> &keys_;
    int index_;

    public:
    OrderedMapIterator(std::map<Key, Value> &values, std::vector<Key> &keys, int index)
        : values_(values), keys_(keys), index_(index){};

    bool operator==(const OrderedMapIterator &other) const { return index_ == other.index_; }
    bool operator!=(const OrderedMapIterator &other) const { return index_ != other.index_; }
    OrderedMapIterator &operator++()
    {
        index_++;
        return *this;
    }
    std::tuple<Key, Value> operator*() { return {keys_[index_], values_[keys_[index_]]}; }
};

template <typename Key, typename Value> class ordered_map
{

    private:
    int counter_ = 0;
    std::vector<Key> keys_;
    std::map<Key, Value> values_;
    void insistKey_(const Key &key)
    {
        if (std::find(keys_.begin(), keys_.end(), key) == keys_.end())
            keys_.push_back(key);
    }

    public:
    ordered_map(){};
    // template <typename Iter> ordered_map(Iter begin, Iter end)
    // {
    //     for (auto i = begin; i <= end; i++)
    //     {
    //         insert(i.first, i.second);
    //     }
    // };
    OrderedMapIterator<Key, Value> begin() { return OrderedMapIterator<Key, Value>(values_, keys_, 0); }
    OrderedMapIterator<Key, Value> end() { return OrderedMapIterator<Key, Value>(values_, keys_, keys_.size()); }
    auto count(const Key &key) const { return values_.count(key); }
    Value &at(const Key &key) const { return values_.at(key); }
    Value &operator[](const Key &key)
    {
        insistKey_(key);
        return values_[key];
    }
    Value &operator[](Key &&key)
    {
        insistKey_(key);
        return values_[key];
    }
};

} // namespace dissolve
