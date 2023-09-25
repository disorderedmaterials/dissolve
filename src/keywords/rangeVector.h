// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "math/range.h"
#include "module/module.h"

// Module Vector Keyword
class RangeVectorKeyword : public KeywordBase
{
    public:
    RangeVectorKeyword(std::vector<Range> &data);
    ~RangeVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data vector
    std::vector<Range> &data_;

    public:
    // Return the data vector
    std::vector<Range> &data();
    const std::vector<Range> &data() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
    // Has not changed from initial value
    bool isDefault() const override;
};