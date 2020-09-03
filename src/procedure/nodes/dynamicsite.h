/*
    *** Procedure Node - Dynamic Site
    *** src/procedure/nodes/dynamicsite.h
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
#include "templates/array.h"
#include "templates/refdatalist.h"
#include "templates/reflist.h"
#include <memory>
#include <vector>

// Forward Declarations
class SelectProcedureNode;
class AtomType;
class Element;
class LineParser;
class Molecule;
class NodeScopeStack;

// Procedure Node - Dynamic Site
class DynamicSiteProcedureNode : public ProcedureNode
{
    public:
    DynamicSiteProcedureNode(SelectProcedureNode *parent);
    ~DynamicSiteProcedureNode();

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context);
    // Return whether a name for the node must be provided
    bool mustBeNamed() const;

    /*
     * Site Criteria
     */
    private:
    // Parent Select node for context
    SelectProcedureNode *parent_;
    // Target Elements for selection as sites
    RefList<Element> elements_;
    // Target AtomTypes for selection as sites
    std::vector<std::shared_ptr<AtomType>> atomTypes_;

    public:
    // Return whether axes are specified for the dynamic site
    bool hasAxes() const { return false; }

    /*
     * Site Generation
     */
    private:
    // List of generated sites
    Array<Site> generatedSites_;

    private:
    // Generate dynamic sites from the specified Molecule
    void generateSites(std::shared_ptr<const Molecule> molecule);

    public:
    // Return Array of generated sites
    const Array<Site> &generatedSites() const;

    /*
     * Execute
     */
    public:
    // Execute node, targetting the supplied Configuration
    ProcedureNode::NodeExecutionResult execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                               GenericList &targetList);
};
