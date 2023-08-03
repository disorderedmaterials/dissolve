// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class Task;

class TaskKeyword : public KeywordBase
{
    public:
    explicit TaskKeyword(const Task *&data);
    ~TaskKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    const Task *&data_;

    public:
    // Return reference to data
    const Task *&data();
    const Task *&data() const;
    void setData(Task *data);

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
};