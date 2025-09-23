// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <memory>
#include <vector>
#include <functional>
#include <optional>

// Data History
template <class T> class History
{
    private:
    // Stored historical data
    std::vector<std::unique_ptr<T>> history_;

    public:
    // Update history with supplied data and return current average
    T average(const T &currentData, int averagingLength, std::function<T()> initialiser = {})
    {
        // Push the current data onto the history stack
        history_.emplace_back(std::make_unique<T>(currentData));

        // Prune old data to get to the averagingLength
        while (history_.size() > averagingLength)
            history_.erase(history_.begin());

        // Perform averaging of the datasets that we have
        T averaged = [&]()
        {
            if (initialiser)
                return initialiser();
            else
                return T();
        }();

        auto weight = 1.0 / history_.size();
        for (auto &data : history_)
            averaged += *data * weight;

        return averaged;
    };
    // Express data as a serialisable value
    SerialisedValue serialise()
        requires(std::is_base_of_v<Serialisable<>, T>)
    {
        SerialisedValue result;
        result["size"] = history_.size();
        Serialisable<>::fromVectorToTable(history_, "data", result);
        return result;
    }
};
