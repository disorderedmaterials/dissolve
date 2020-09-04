/*
    *** Procedure Node - Calculate Vector
    *** src/procedure/nodes/calculatevector.h
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

#include "procedure/nodes/calculatebase.h"
#include "templates/array.h"

// Forward Declarations
class SelectProcedureNode;

// Calculate Vector Node
class CalculateVectorProcedureNode : public CalculateProcedureNodeBase
{
    public:
    CalculateVectorProcedureNode(SelectProcedureNode *site0 = nullptr, SelectProcedureNode *site1 = nullptr,
                                 bool rotateIntoFrame = false);
    ~CalculateVectorProcedureNode();

    /*
     * Data
     */
    private:
    // Whether to rotate into the frame of the first site (retrieved from keyword)
    bool rotateIntoFrame_;

    /*
     * Observable Target (implements virtuals in CalculateProcedureNodeBase)
     */
    public:
    // Return number of sites required to calculate observable
    int nSitesRequired() const;
    // Return dimensionality of calculated observable
    int dimensionality() const;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList);
    // Execute node, targetting the supplied Configuration
    ProcedureNode::NodeExecutionResult execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                               GenericList &targetList);
};
