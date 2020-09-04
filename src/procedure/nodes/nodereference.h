/*
    *** Procedure Node Reference
    *** src/procedure/nodes/nodereference.h
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
#include "templates/listitem.h"

// Forward Declarations
class CoreData;
class AnalyseModule;

// Procedure Node Reference
class ProcedureNodeReference : public ListItem<ProcedureNodeReference>
{
    public:
    ProcedureNodeReference(ProcedureNode *node = nullptr);
    ~ProcedureNodeReference();

    /*
     * Data
     */
    private:
    // Target node
    ProcedureNode *node_;
    // Parent AnalyseModule owning the node (if relevant)
    AnalyseModule *analyseModuleParent_;
    // Allowable node types (when reading / setting)
    bool allowedTypes_[ProcedureNode::nNodeTypes];

    public:
    // Return target node
    ProcedureNode *node();
    // Return node type
    ProcedureNode::NodeType type() const;
    // Add allowable node type
    void addAllowableNodeType(ProcedureNode::NodeType nt);
    // Allow all node types
    void setAllowAllNodeTypes();
    // Return if node pointer is NULL
    bool isNull() const;

    /*
     * Operators
     */
    public:
    void operator=(ProcedureNode *node);
    void operator=(const ProcedureNodeReference &nodeRef);

    /*
     * Read / Write
     */
    public:
    // Read structure from specified LineParser
    bool read(LineParser &parser, int startArg, CoreData &coreData, const Procedure *procedure);
    // Write structure to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);
};
