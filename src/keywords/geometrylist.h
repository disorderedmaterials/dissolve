// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/geometry.h"
#include "keywords/base.h"

// Keyword managing vector of Geometry Data
class GeometryListKeyword : public KeywordBase
{
    public:
    GeometryListKeyword(std::vector<Geometry> &data, Geometry::GeometryType geometryType);
    ~GeometryListKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to vector of data
    std::vector<Geometry> &data_;
    // Geometry type accepted
    Geometry::GeometryType geometryType_;

    public:
    // Return reference to vector of data
    std::vector<Geometry> &data();
    const std::vector<Geometry> &data() const;

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
};
