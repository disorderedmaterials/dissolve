// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <functional>
#include <memory>
#include <vector>

// Serialisable Data History
// Requires that the template class T is itself a Serialisable
template <class T> class History : public Serialisable<>
{
    public:
    History() = default;
    History(std::function<T()> initialiser = {}) : initialiser_(std::move(initialiser)) {}

    private:
    // Stored historical data
    std::vector<std::unique_ptr<T>> history_;
    // Object initialisation function (if required)
    std::function<T()> initialiser_{};

    public:
    // Update history with supplied data and return current average
    T average(const T &currentData, int averagingLength)
    {
        // Push the current data onto the history stack
        history_.emplace_back(std::make_unique<T>(currentData));

        // Prune old data to get to the averagingLength
        while (history_.size() > averagingLength)
            history_.erase(history_.begin());

        // Perform averaging of the datasets that we have
        T averaged = initialiser_ ? initialiser_() : T();

        auto weight = 1.0 / history_.size();
        for (auto &data : history_)
            averaged += *data * weight;

        return averaged;
    }

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    SerialisedValue serialise() const
    {
        return Serialisable::fromVector(history_, [&](const auto &itemPtr) { return itemPtr->serialise(); });
    }
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override
    {
        history_.clear();
        return Serialisable::toVector(node,
                                      [&](const auto &value)
                                      {
                                          auto &unique =
                                              history_.emplace_back(std::make_unique<T>(initialiser_ ? initialiser_() : T()));
                                          unique->deserialise(value);
                                      });
    }
};
