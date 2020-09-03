/*
    *** Procedure Node - Sum1D
    *** src/procedure/nodes/sum1d.cpp
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

#include "procedure/nodes/sum1d.h"
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

Sum1DProcedureNode::Sum1DProcedureNode(const Process1DProcedureNode *target) : ProcedureNode(ProcedureNode::Sum1DNode)
{
    keywords_.add("Target", new NodeKeyword<const Process1DProcedureNode>(this, ProcedureNode::Process1DNode, false, target),
                  "SourceData", "Process1D node containing the data to sum");
    keywords_.add("Ranges", new RangeKeyword(Range(0.0, 3.0), Vec3Labels::MinMaxDeltaLabels), "RangeA",
                  "X range for first summation region");
    keywords_.add("Ranges", new BoolKeyword(false), "RangeBEnabled", "Whether the second summation region is enabled");
    keywords_.add("Ranges", new RangeKeyword(Range(3.0, 6.0), Vec3Labels::MinMaxDeltaLabels), "RangeB",
                  "X range for second summation region");
    keywords_.add("Ranges", new BoolKeyword(false), "RangeCEnabled", "Whether the second summation region is enabled");
    keywords_.add("Ranges", new RangeKeyword(Range(6.0, 9.0), Vec3Labels::MinMaxDeltaLabels), "RangeC",
                  "X range for third summation region");
}

Sum1DProcedureNode::~Sum1DProcedureNode() {}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Sum1DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return calculated sum
const SampledDouble &Sum1DProcedureNode::sum(int index) const { return sum_[index]; }

// Return whether range B is enabled (from keyword data)
bool Sum1DProcedureNode::isRangeBEnabled() const { return keywords_.asBool("RangeBEnabled"); }

// Return whether range C is enabled (from keyword data)
bool Sum1DProcedureNode::isRangeCEnabled() const { return keywords_.asBool("RangeCEnabled"); }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Sum1DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Retrieve the Process1D node target
    processNode_ = keywords_.retrieve<const Process1DProcedureNode *>("SourceData");
    if (!processNode_)
        return Messenger::error("No source Process1D node set in '{}'.\n", name());

    // Get ranges and status flags
    rangeA_ = keywords_.retrieve<Range>("RangeA");
    rangeB_ = keywords_.retrieve<Range>("RangeB");
    rangeC_ = keywords_.retrieve<Range>("RangeC");
    rangeBEnabled_ = keywords_.asBool("RangeBEnabled");
    rangeCEnabled_ = keywords_.asBool("RangeCEnabled");

    return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult Sum1DProcedureNode::execute(ProcessPool &procPool, Configuration *cfg,
                                                               std::string_view prefix, GenericList &targetList)
{
    // Calculate integrals
    sum_[0] += Integrator::sum(processNode_->processedData(), rangeA_);
    if (rangeBEnabled_)
        sum_[1] += Integrator::sum(processNode_->processedData(), rangeB_);
    if (rangeCEnabled_)
        sum_[2] += Integrator::sum(processNode_->processedData(), rangeC_);

    // Print info
    Messenger::print("Sum1D - Range A: {:e} +/- {:e} over {:e} < x < {:e}.\n", sum_[0].mean(), sum_[0].stDev(),
                     rangeA_.minimum(), rangeA_.maximum());
    if (rangeBEnabled_)
        Messenger::print("Sum1D - Range B: {:e} +/- {:e} over {:e} < x < {:e}.\n", sum_[1].mean(), sum_[1].stDev(),
                         rangeB_.minimum(), rangeB_.maximum());
    if (rangeCEnabled_)
        Messenger::print("Sum1D - Range C: {:e} +/- {:e} over {:e} < x < {:e}.\n", sum_[2].mean(), sum_[2].stDev(),
                         rangeC_.minimum(), rangeC_.maximum());

    return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool Sum1DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    return true;
}
