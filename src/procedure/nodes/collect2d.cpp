/*
    *** Procedure Node - Collect2D
    *** src/procedure/nodes/collect2d.cpp
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

#include "procedure/nodes/collect2d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "genericitems/listhelper.h"
#include "keywords/types.h"
#include "math/data2d.h"
#include "procedure/nodes/calculatebase.h"
#include "procedure/nodes/sequence.h"

Collect2DProcedureNode::Collect2DProcedureNode(CalculateProcedureNodeBase *xObservable, CalculateProcedureNodeBase *yObservable,
                                               double xMin, double xMax, double xBinWidth, double yMin, double yMax,
                                               double yBinWidth)
    : ProcedureNode(ProcedureNode::Collect2DNode)
{
    keywords_.add(
        "Target",
        new NodeAndIntegerKeyword<CalculateProcedureNodeBase>(this, ProcedureNode::CalculateBaseNode, true, xObservable, 0),
        "QuantityX", "Calculated observable to collect for x axis");
    keywords_.add(
        "Target",
        new NodeAndIntegerKeyword<CalculateProcedureNodeBase>(this, ProcedureNode::CalculateBaseNode, true, yObservable, 0),
        "QuantityY", "Calculated observable to collect for y axis");
    keywords_.add("Target",
                  new Vec3DoubleKeyword(Vec3<double>(xMin, xMax, xBinWidth), Vec3<double>(0.0, 0.0, 1.0e-5),
                                        Vec3Labels::MinMaxBinwidthlabels),
                  "RangeX", "Range of calculation for the specified x observable");
    keywords_.add("Target",
                  new Vec3DoubleKeyword(Vec3<double>(yMin, yMax, yBinWidth), Vec3<double>(0.0, 0.0, 1.0e-5),
                                        Vec3Labels::MinMaxDeltaLabels),
                  "RangeY", "Range of calculation for the specified y observable");
    keywords_.add("HIDDEN", new NodeBranchKeyword(this, &subCollectBranch_, ProcedureNode::AnalysisContext), "SubCollect",
                  "Branch which runs if the target quantities were binned successfully");

    // Initialise branch
    subCollectBranch_ = nullptr;
}

Collect2DProcedureNode::~Collect2DProcedureNode() {}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Collect2DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return accumulated data
const Data2D &Collect2DProcedureNode::accumulatedData() const
{
    if (!histogram_)
    {
        Messenger::error("No histogram pointer set in Collect2DProcedureNode, so no accumulated data to return.\n");
        static Data2D dummy;
        return dummy;
    }

    return histogram_->accumulatedData();
}

// Return x range minimum
double Collect2DProcedureNode::xMinimum() const { return keywords_.asVec3Double("RangeX").x; }

// Return x range maximum
double Collect2DProcedureNode::xMaximum() const { return keywords_.asVec3Double("RangeX").y; }

// Return x bin width
double Collect2DProcedureNode::xBinWidth() const { return keywords_.asVec3Double("RangeX").z; }

// Return y range minimum
double Collect2DProcedureNode::yMinimum() const { return keywords_.asVec3Double("RangeY").x; }

// Return y range maximum
double Collect2DProcedureNode::yMaximum() const { return keywords_.asVec3Double("RangeY").y; }

// Return y bin width
double Collect2DProcedureNode::yBinWidth() const { return keywords_.asVec3Double("RangeY").z; }

/*
 * Branches
 */

// Add and return subcollection sequence branch
SequenceProcedureNode *Collect2DProcedureNode::addSubCollectBranch(ProcedureNode::NodeContext context)
{
    if (!subCollectBranch_)
        subCollectBranch_ = new SequenceProcedureNode(context, procedure());

    return subCollectBranch_;
}

// Return whether this node has a branch
bool Collect2DProcedureNode::hasBranch() const { return (subCollectBranch_ != nullptr); }
// Return SequenceNode for the branch (if it exists)
SequenceProcedureNode *Collect2DProcedureNode::branch() { return subCollectBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Collect2DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Construct our data name, and search for it in the supplied list
    std::string dataName = fmt::format("{}_{}_Bins", name(), cfg->niceName());
    bool created;
    auto &target =
        GenericListHelper<Histogram2D>::realise(targetList, dataName, prefix, GenericItem::InRestartFileFlag, &created);
    if (created)
    {
        Messenger::printVerbose("Two-dimensional histogram data for '{}' was not in the target list, so it will now be "
                                "initialised...\n",
                                name());
        target.initialise(xMinimum(), xMaximum(), xBinWidth(), yMinimum(), yMaximum(), yBinWidth());
    }

    // Zero the current bins, ready for the new pass
    target.zeroBins();

    // Store a pointer to the data
    histogram_ = &target;

    // Retrieve the observables
    std::tie(xObservable_, xObservableIndex_) = keywords_.retrieve<std::tuple<CalculateProcedureNodeBase *, int>>("QuantityX");
    if (!xObservable_)
        return Messenger::error("No valid x quantity set in '{}'.\n", name());
    std::tie(yObservable_, yObservableIndex_) = keywords_.retrieve<std::tuple<CalculateProcedureNodeBase *, int>>("QuantityY");
    if (!yObservable_)
        return Messenger::error("No valid y quantity set in '{}'.\n", name());

    // Prepare any branches
    if (subCollectBranch_ && (!subCollectBranch_->prepare(cfg, prefix, targetList)))
        return false;

    return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult Collect2DProcedureNode::execute(ProcessPool &procPool, Configuration *cfg,
                                                                   std::string_view prefix, GenericList &targetList)
{
#ifdef CHECKS
    if (!xObservable_)
    {
        Messenger::error("No CalculateProcedureNodeBase pointer set for X observable in Collect2DProcedureNode '{}'.\n",
                         name());
        return ProcedureNode::Failure;
    }
    if (!yObservable_)
    {
        Messenger::error("No CalculateProcedureNodeBase pointer set for Y observable in Collect2DProcedureNode '{}'.\n",
                         name());
        return ProcedureNode::Failure;
    }
#endif
    // Bin the current value of the observable
    if (histogram_->bin(xObservable_->value(xObservableIndex_), yObservable_->value(yObservableIndex_)) && subCollectBranch_)
        return subCollectBranch_->execute(procPool, cfg, prefix, targetList);

    return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool Collect2DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                      GenericList &targetList)
{
#ifdef CHECKS
    if (!histogram_)
    {
        Messenger::error("No Data2D pointer set in Collect2DProcedureNode '{}'.\n", name());
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
