/*
    *** Keyword - NodeValue
    *** src/keywords/nodevalue.h
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

#include "keywords/data.h"
#include "procedure/nodevalue.h"

// Forward Declarations
class NodeValue;
class ProcedureNode;

// Keyword with NodeValue
class NodeValueKeyword : public KeywordData<NodeValue>
{
    public:
    NodeValueKeyword(ProcedureNode *parentNode, NodeValue value);
    ~NodeValueKeyword();

    /*
     * Parent Node
     */
    private:
    // Parent ProcedureNode
    ProcedureNode *parentNode_;

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

    /*
     * Set
     */
    public:
    // Set the value from supplied expression text
    bool setValue(std::string_view expressionText);

    /*
     * Conversion
     */
    public:
    // Return value (as int)
    int asInt();
    // Return value (as double)
    double asDouble();

    /*
     * Object Management
     */
    protected:
};
