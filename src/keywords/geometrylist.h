// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/geometry.h"
#include "keywords/data.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// Keyword with Geometry Data
class GeometryListKeyword : public KeywordData<List<Geometry> &>
{
    public:
    GeometryListKeyword(List<Geometry> &, Geometry::GeometryType t);

    ~GeometryListKeyword();

    /*
     * Data
     */
    private:
    // enum variable
    Geometry::GeometryType type_;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const;
    // Return maximum number of arguments accepted
    int maxArguments() const;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, CoreData &coreData);
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix);
};
