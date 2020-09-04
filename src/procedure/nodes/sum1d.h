/*
    *** Procedure Node - Sum1D
    *** src/procedure/nodes/sum1d.h
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

#include "math/range.h"
#include "math/sampleddouble.h"
#include "procedure/nodes/node.h"
#include "templates/reflist.h"

// Forward Declarations
class Process1DProcedureNode;
class LineParser;
class NodeScopeStack;

// Procedure Node - Sum1D
class Sum1DProcedureNode : public ProcedureNode
{
    public:
    Sum1DProcedureNode(const Process1DProcedureNode *target = nullptr);
    ~Sum1DProcedureNode();

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context);

    /*
     * Data
     */
    private:
    // Process1D node that we are targetting (retrieved from keyword 'SourceData')
    const Process1DProcedureNode *processNode_;
    // Ranges for sums (retrieved from keywords)
    Range rangeA_, rangeB_, rangeC_;
    // Flags for second and third ranges
    bool rangeBEnabled_, rangeCEnabled_;
    // Calculated sums (stored in processing data list)
    SampledDouble sum_[3];

    public:
    // Return calculated sum specified
    const SampledDouble &sum(int index) const;
    // Return whether range B is enabled (from keyword data)
    bool isRangeBEnabled() const;
    // Return whether range C is enabled (from keyword data)
    bool isRangeCEnabled() const;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList);
    // Execute node, targetting the supplied Configuration
    ProcedureNode::NodeExecutionResult execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                               GenericList &targetList);
    // Finalise any necessary data after execution
    bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList);
};
