// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <functional>
#include <memory>
#include <vector>

// Serialisable Data History
// Requires that the template class T is itself a Serialisable and implements the += and * operators
template <class T> class History : public Serialisable<>
{
    public:
    History(std::function<T()> initialiser = {}) : initialiser_(std::move(initialiser)) {}

    private:
    // Stored historical data
    std::vector<std::unique_ptr<T>> history_;
    // Object initialisation function (if required)
    std::function<T()> initialiser_{};

    public:
    // Push data into the history and return current average
    T push(const T &data, int averagingLength)
    {
        // Push the current data onto the history stack
        history_.emplace_back(std::make_unique<T>(data));

        // Prune old data to get to the averagingLength
        while (history_.size() > averagingLength)
            history_.erase(history_.begin());

        return average();
    }
    // Return the current average value
    T average() const
    {
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
    void serialise(std::string tag, SerialisedValue &target) const
    {
        return Serialisable::fromVector(history_, tag, target, [&](const auto &itemPtr) { return itemPtr->into_toml(); });
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

// Serialisable POD Data History
// History for PODs, e.g. double, int
template <class T> class PODHistory : public Serialisable<>
{
    private:
    // Stored historical data
    std::vector<T> history_;

    public:
    // Push data into the history
    void push(const T &data, int averagingLength)
    {
        // Push the current data onto the history stack
        history_.emplace_back(data);

        // Prune old data to get to the averagingLength
        while (history_.size() > averagingLength)
            history_.erase(history_.begin());
    }
    // Push data into the history and return current average
    T pushAndAverage(const T &data, int averagingLength)
    {
        push(data, averagingLength);
        return average();
    }
    // Return the current average value
    T average() const
    {
        // Perform averaging of the datasets that we have
        T averaged = T();

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
    void serialise(std::string tag, SerialisedValue &target) const
    {
        if (history_.empty())
            return;

        SerialisedValue data = {{"history", history_}};
        target[tag] = data;
    }
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override { history_ = toml::find<std::vector<T>>(node, "history"); }
};
