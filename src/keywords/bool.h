// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
    const bool default_;
    // Whether the data has been set
    bool set_{false};

    public:
    // See if the value has changed
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
    SerialisedValue serialise() const override;
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
