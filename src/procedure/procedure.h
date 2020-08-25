/*
    *** Procedure
    *** src/procedure/procedure.h
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

#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"
#include "templates/refdatalist.h"

// Forward Declarations
class Configuration;
class LineParser;
class ProcessPool;

// Procedure
class Procedure
{
    public:
    Procedure(ProcedureNode::NodeContext context, std::string_view blockTerminationKeyword = "EndProcedure");
    ~Procedure();

    /*
     * Data
     */
    private:
    // Context for the main Procedure
    ProcedureNode::NodeContext context_;
    // Sequence node from which the Procedure starts
    SequenceProcedureNode rootSequence_;

    public:
    // Clear all data
    void clear();
    // Add (own) specified node to root sequence
    void addRootSequenceNode(ProcedureNode *node);
    // Return root sequence
    const SequenceProcedureNode &rootSequence() const;
    // Return the block termination keyword for the Procedure
    std::string_view blockTerminationKeyword() const;
    // Return named node if present, and which matches the (optional) type given
    ProcedureNode *node(std::string_view name, ProcedureNode::NodeType nt = ProcedureNode::nNodeTypes) const;

    /*
     * Execute
     */
    private:
    // List of Configurations and the coordinate indices at which they were last processed
    RefDataList<Configuration, int> configurationPoints_;

    public:
    // Run procedure on specified Configuration, storing / retrieving generated data from supplied list
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList);

    /*
     * Read / Write
     */
    public:
    // Read procedure from specified LineParser
    bool read(LineParser &parser, CoreData &coreData);
    // Write procedure to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);
};
