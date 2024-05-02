// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Keyword managing Bool data
class BoolKeyword : public KeywordBase
{
    public:
    explicit BoolKeyword(bool &data);
    ~BoolKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to target data
    bool &data_;
    // Initial value
    const bool default_;
    // Whether the data has been set
    bool set_{false};

    public:
    // Has not changed from initial value
    bool isDefault() const override;
    // Set data
    bool setData(bool value);
    // Return data
    const bool &data() const;

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
