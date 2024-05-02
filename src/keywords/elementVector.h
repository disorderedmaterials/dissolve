// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include "keywords/base.h"

// Keyword managing Element vector data
class ElementVectorKeyword : public KeywordBase
{
    public:
    explicit ElementVectorKeyword(std::vector<Elements::Element> &data);
    ~ElementVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    std::vector<Elements::Element> &data_;

    public:
    // Return reference to data
    std::vector<Elements::Element> &data();
    const std::vector<Elements::Element> &data() const;

    /*
     * Arguments
     */
    public:
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
};
