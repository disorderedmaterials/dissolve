/*
    *** Procedure Node - Integrate1D
    *** src/procedure/nodes/integrate1d.cpp
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

#include "procedure/nodes/integrate1d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "genericitems/listhelper.h"
#include "io/export/data1d.h"
#include "keywords/types.h"
#include "math/integrator.h"
#include "modules/analyse/analyse.h"
#include "procedure/nodes/operatebase.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/select.h"

Integrate1DProcedureNode::Integrate1DProcedureNode(const Process1DProcedureNode *target)
    : ProcedureNode(ProcedureNode::Integrate1DNode)
{
    keywords_.add("Target", new NodeKeyword<const Process1DProcedureNode>(this, ProcedureNode::Process1DNode, false, target),
                  "SourceData", "Process1D node containing the data to integrate");
    keywords_.add("Ranges", new RangeKeyword(Range(0.0, 3.0), Vec3Labels::MinMaxDeltaLabels), "RangeA",
                  "X range for first integration region");
    keywords_.add("Ranges", new RangeKeyword(Range(3.0, 6.0), Vec3Labels::MinMaxDeltaLabels), "RangeB",
                  "X range for second integration region");
    keywords_.add("Ranges", new RangeKeyword(Range(6.0, 9.0), Vec3Labels::MinMaxDeltaLabels), "RangeC",
                  "X range for third integration region");
}

Integrate1DProcedureNode::~Integrate1DProcedureNode() {}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Integrate1DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return calculated integral
const SampledDouble &Integrate1DProcedureNode::integral(int index) const { return integral_[index]; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Integrate1DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Retrieve the Process1D node target
    processNode_ = keywords_.retrieve<const Process1DProcedureNode *>("SourceData");
    if (!processNode_)
        return Messenger::error("No source Process1D node set in '{}'.\n", name());

    return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult Integrate1DProcedureNode::execute(ProcessPool &procPool, Configuration *cfg,
                                                                     std::string_view prefix, GenericList &targetList)
{
    // Get ranges
    auto rangeA = keywords_.retrieve<Range>("RangeA");
    auto rangeB = keywords_.retrieve<Range>("RangeB");
    auto rangeC = keywords_.retrieve<Range>("RangeC");

    // Calculate integrals
    integral_[0] += Integrator::trapezoid(processNode_->processedData(), keywords_.retrieve<Range>("RangeA"));
    integral_[1] += Integrator::trapezoid(processNode_->processedData(), keywords_.retrieve<Range>("RangeB"));
    integral_[2] += Integrator::trapezoid(processNode_->processedData(), keywords_.retrieve<Range>("RangeC"));

    // Print info
    Messenger::print("Integrate1D - Range A: {:e} +/- {:e} over {:e} < x < {:e}.\n", integral_[0].mean(), integral_[0].stDev(),
                     rangeA.minimum(), rangeA.maximum());
    Messenger::print("Integrate1D - Range B: {:e} +/- {:e} over {:e} < x < {:e}.\n", integral_[1].mean(), integral_[1].stDev(),
                     rangeB.minimum(), rangeB.maximum());
    Messenger::print("Integrate1D - Range C: {:e} +/- {:e} over {:e} < x < {:e}.\n", integral_[2].mean(), integral_[2].stDev(),
                     rangeC.minimum(), rangeC.maximum());

    return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool Integrate1DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                        GenericList &targetList)
{
    return true;
}
