// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include <memory>
#include <vector>

// Data Averaging
template <class T> class Averager
{
    private:
    // Historical data
    std::vector<std::unique_ptr<T>> history_;

    public:
    // Update history with supplied data and return current average
    T average(const T &currentData, int averagingLength)
    {
        // Push the current data onto the history stack
        history_.emplace_back(std::make_unique<T>(currentData));

        // Prune old data to get to the averagingLength
        while (history_.size() > averagingLength)
            history_.pop_front();

        // Perform averaging of the datasets that we have
        T averaged;
        auto weight = 1.0 / history_.size();
        for (auto &data : history_)
            averaged += *data * weight;

        return averaged;
    };
};
