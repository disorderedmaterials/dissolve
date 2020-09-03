/*
    *** Keyword - Geometry List
    *** src/keywords/geometrylist.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
