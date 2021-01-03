// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <algorithm>
#include <functional>
#include <vector>

template <class T> class OrderedVector : std::vector<T>
{
    private:
    std::function<bool(T, T)> compare_ = std::less<T>();
    using std::vector<T>::back;
    using std::vector<T>::emplace_back;
    using std::vector<T>::erase;
    using std::vector<T>::insert;
    using std::vector<T>::pop_back;
    using std::vector<T>::push_back;

    public:
    using std::vector<T>::begin;
    using std::vector<T>::clear;
    using std::vector<T>::end;
    using std::vector<T>::size;
    OrderedVector() : std::vector<T>(){};
    OrderedVector(std::function<bool(T, T)> comp) : compare_(comp){};
    void insert(const T &value)
    {
        auto location = std::lower_bound(begin(), end(), value, compare_);
        if (location != end() && *location == value)
            return;
        insert(location, value);
    }

    template <class... Args> void emplace(Args &&... args)
    {
        emplace_back(args...);
        T &item = back();
        pop_back();
        insert(item);
    }

    bool erase(T &value)
    {
        auto location = std::lower_bound(begin(), end(), value, compare_);
        if (location == end() || *location != value)
            return false;
        erase(location);
        return true;
    }
};
