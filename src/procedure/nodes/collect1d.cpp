/*
    *** Procedure Node - Collect1D
    *** src/procedure/nodes/collect1d.cpp
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

#include "procedure/nodes/collect1d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "genericitems/listhelper.h"
#include "keywords/types.h"
#include "math/data1d.h"
#include "procedure/nodes/calculatebase.h"
#include "procedure/nodes/sequence.h"

Collect1DProcedureNode::Collect1DProcedureNode(CalculateProcedureNodeBase *observable, double rMin, double rMax,
                                               double binWidth)
    : ProcedureNode(ProcedureNode::Collect1DNode)
{
    keywords_.add(
        "Target",
        new NodeAndIntegerKeyword<CalculateProcedureNodeBase>(this, ProcedureNode::CalculateBaseNode, true, observable, 0),
        "QuantityX", "Calculated observable to collect");
    keywords_.add("Target",
                  new Vec3DoubleKeyword(Vec3<double>(rMin, rMax, binWidth), Vec3<double>(0.0, 0.0, 1.0e-5),
                                        Vec3Labels::MinMaxBinwidthlabels),
                  "RangeX", "Range of calculation for the specified observable");
    keywords_.add("HIDDEN", new NodeBranchKeyword(this, &subCollectBranch_, ProcedureNode::AnalysisContext), "SubCollect",
                  "Branch which runs if the target quantity was binned successfully");

    // Initialise branch
    subCollectBranch_ = nullptr;
}

Collect1DProcedureNode::~Collect1DProcedureNode() {}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Collect1DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return accumulated data
const Data1D &Collect1DProcedureNode::accumulatedData() const
{
    if (!histogram_)
    {
        Messenger::error("No histogram pointer set in Collect1DProcedureNode, so no accumulated data to return.\n");
        static Data1D dummy;
        return dummy;
    }

    return histogram_->accumulatedData();
}

// Return range minimum
double Collect1DProcedureNode::minimum() const { return keywords_.asVec3Double("RangeX").x; }

// Return range maximum
double Collect1DProcedureNode::maximum() const { return keywords_.asVec3Double("RangeX").y; }

// Return bin width
double Collect1DProcedureNode::binWidth() const { return keywords_.asVec3Double("RangeX").z; }

/*
 * Branches
 */

// Add and return subcollection sequence branch
SequenceProcedureNode *Collect1DProcedureNode::addSubCollectBranch(ProcedureNode::NodeContext context)
{
    if (!subCollectBranch_)
        subCollectBranch_ = new SequenceProcedureNode(context, procedure());

    return subCollectBranch_;
}

// Return whether this node has a branch
bool Collect1DProcedureNode::hasBranch() const { return (subCollectBranch_ != nullptr); }

// Return SequenceNode for the branch (if it exists)
SequenceProcedureNode *Collect1DProcedureNode::branch() { return subCollectBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Collect1DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Construct our data name, and search for it in the supplied list
    std::string dataName = fmt::format("{}_{}_Bins", name(), cfg->niceName());
    bool created;
    auto &target =
        GenericListHelper<Histogram1D>::realise(targetList, dataName, prefix, GenericItem::InRestartFileFlag, &created);
    if (created)
    {
        Messenger::printVerbose("One-dimensional histogram data for '{}' was not in the target list, so it will now be "
                                "initialised...\n",
                                name());
        target.initialise(minimum(), maximum(), binWidth());
    }

    // Zero the current bins, ready for the new pass
    target.zeroBins();

    // Store a pointer to the data
    histogram_ = &target;

    // Retrieve the observable
    std::tie(xObservable_, xObservableIndex_) = keywords_.retrieve<std::tuple<CalculateProcedureNodeBase *, int>>("QuantityX");
    if (!xObservable_)
        return Messenger::error("No valid x quantity set in '{}'.\n", name());

    // Prepare any branches
    if (subCollectBranch_ && (!subCollectBranch_->prepare(cfg, prefix, targetList)))
        return false;

    return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult Collect1DProcedureNode::execute(ProcessPool &procPool, Configuration *cfg,
                                                                   std::string_view prefix, GenericList &targetList)
{
#ifdef CHECKS
    if (!xObservable_)
    {
        Messenger::error("No CalculateProcedureNodeBase pointer set in Collect1DProcedureNode '{}'.\n", name());
        return ProcedureNode::Failure;
    }
#endif
    // Bin the current value of the observable, and execute sub-collection branch on success
    if (histogram_->bin(xObservable_->value(xObservableIndex_)) && subCollectBranch_)
        return subCollectBranch_->execute(procPool, cfg, prefix, targetList);

    return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool Collect1DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                      GenericList &targetList)
{
#ifdef CHECKS
    if (!histogram_)
    {
        Messenger::error("No Data1D pointer set in Collect1DProcedureNode '{}'.\n", name());
        return ProcedureNode::Failure;
    }
#endif
    // Accumulate the current binned data
    histogram_->accumulate();

    // Finalise any branches
    if (subCollectBranch_ && (!subCollectBranch_->finalise(procPool, cfg, prefix, targetList)))
        return false;

    return true;
}
