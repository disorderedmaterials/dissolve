/*
    *** Procedure Node - Collect3D
    *** src/procedure/nodes/collect3d.cpp
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

#include "procedure/nodes/collect3d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "genericitems/listhelper.h"
#include "keywords/types.h"
#include "math/data3d.h"
#include "procedure/nodes/calculatebase.h"
#include "procedure/nodes/sequence.h"

Collect3DProcedureNode::Collect3DProcedureNode(CalculateProcedureNodeBase *xObservable, CalculateProcedureNodeBase *yObservable,
                                               CalculateProcedureNodeBase *zObservable, double xMin, double xMax,
                                               double xBinWidth, double yMin, double yMax, double yBinWidth, double zMin,
                                               double zMax, double zBinWidth)
    : ProcedureNode(ProcedureNode::Collect3DNode)
{
    keywords_.add(
        "Target",
        new NodeAndIntegerKeyword<CalculateProcedureNodeBase>(this, ProcedureNode::CalculateBaseNode, true, xObservable, 0),
        "QuantityX", "Calculated observable to collect for x axis");
    keywords_.add(
        "Target",
        new NodeAndIntegerKeyword<CalculateProcedureNodeBase>(this, ProcedureNode::CalculateBaseNode, true, yObservable, 0),
        "QuantityY", "Calculated observable to collect for y axis");
    keywords_.add(
        "Target",
        new NodeAndIntegerKeyword<CalculateProcedureNodeBase>(this, ProcedureNode::CalculateBaseNode, true, zObservable, 0),
        "QuantityZ", "Calculated observable to collect for z axis");
    keywords_.add("Target",
                  new Vec3DoubleKeyword(Vec3<double>(xMin, xMax, xBinWidth), Vec3<double>(-1.0e6, -1.0e6, 0.001),
                                        Vec3Labels::MinMaxDeltaLabels),
                  "RangeX", "Range of calculation for the specified x observable");
    keywords_.add("Target",
                  new Vec3DoubleKeyword(Vec3<double>(yMin, yMax, yBinWidth), Vec3<double>(-1.0e6, -1.0e6, 0.001),
                                        Vec3Labels::MinMaxDeltaLabels),
                  "RangeY", "Range of calculation for the specified y observable");
    keywords_.add("Target",
                  new Vec3DoubleKeyword(Vec3<double>(zMin, zMax, zBinWidth), Vec3<double>(-1.0e6, -1.0e6, 0.0015),
                                        Vec3Labels::MinMaxDeltaLabels),
                  "RangeZ", "Range of calculation for the specified z observable");
    keywords_.add("HIDDEN", new NodeBranchKeyword(this, &subCollectBranch_, ProcedureNode::AnalysisContext), "SubCollect",
                  "Branch which runs if the target quantities were binned successfully");

    // Initialise branch
    subCollectBranch_ = nullptr;
}
Collect3DProcedureNode::Collect3DProcedureNode(CalculateProcedureNodeBase *xyzObservable, double xMin, double xMax,
                                               double xBinWidth, double yMin, double yMax, double yBinWidth, double zMin,
                                               double zMax, double zBinWidth)
    : ProcedureNode(ProcedureNode::Collect3DNode)
{
    keywords_.add(
        "Target",
        new NodeAndIntegerKeyword<CalculateProcedureNodeBase>(this, ProcedureNode::CalculateBaseNode, true, xyzObservable, 0),
        "QuantityX", "Calculated observable to collect for x axis");
    keywords_.add(
        "Target",
        new NodeAndIntegerKeyword<CalculateProcedureNodeBase>(this, ProcedureNode::CalculateBaseNode, true, xyzObservable, 1),
        "QuantityY", "Calculated observable to collect for y axis");
    keywords_.add(
        "Target",
        new NodeAndIntegerKeyword<CalculateProcedureNodeBase>(this, ProcedureNode::CalculateBaseNode, true, xyzObservable, 2),
        "QuantityZ", "Calculated observable to collect for z axis");
    keywords_.add("Target",
                  new Vec3DoubleKeyword(Vec3<double>(xMin, xMax, xBinWidth), Vec3<double>(-1.0e6, -1.0e6, 0.001),
                                        Vec3Labels::MinMaxDeltaLabels),
                  "RangeX", "Range of calculation for the specified x observable");
    keywords_.add("Target",
                  new Vec3DoubleKeyword(Vec3<double>(yMin, yMax, yBinWidth), Vec3<double>(-1.0e6, -1.0e6, 0.001),
                                        Vec3Labels::MinMaxDeltaLabels),
                  "RangeY", "Range of calculation for the specified y observable");
    keywords_.add("Target",
                  new Vec3DoubleKeyword(Vec3<double>(zMin, zMax, zBinWidth), Vec3<double>(-1.0e6, -1.0e6, 0.001),
                                        Vec3Labels::MinMaxDeltaLabels),
                  "RangeZ", "Range of calculation for the specified z observable");
    keywords_.add("HIDDEN", new NodeBranchKeyword(this, &subCollectBranch_, ProcedureNode::AnalysisContext), "SubCollect",
                  "Branch which runs if the target quantities were binned successfully");

    // Initialise branch
    subCollectBranch_ = nullptr;
}

Collect3DProcedureNode::~Collect3DProcedureNode() {}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Collect3DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return accumulated data
const Data3D &Collect3DProcedureNode::accumulatedData() const
{
    if (!histogram_)
    {
        Messenger::error("No histogram pointer set in Collect3DProcedureNode, so no accumulated data to return.\n");
        static Data3D dummy;
        return dummy;
    }

    return histogram_->accumulatedData();
}

// Return x range minimum
double Collect3DProcedureNode::xMinimum() const { return keywords_.asVec3Double("RangeX").x; }

// Return x range maximum
double Collect3DProcedureNode::xMaximum() const { return keywords_.asVec3Double("RangeX").y; }

// Return x bin width
double Collect3DProcedureNode::xBinWidth() const { return keywords_.asVec3Double("RangeX").z; }

// Return y range minimum
double Collect3DProcedureNode::yMinimum() const { return keywords_.asVec3Double("RangeY").x; }

// Return y range maximum
double Collect3DProcedureNode::yMaximum() const { return keywords_.asVec3Double("RangeY").y; }

// Return y bin width
double Collect3DProcedureNode::yBinWidth() const { return keywords_.asVec3Double("RangeY").z; }

// Return z range minimum
double Collect3DProcedureNode::zMinimum() const { return keywords_.asVec3Double("RangeZ").x; }

// Return z range maximum
double Collect3DProcedureNode::zMaximum() const { return keywords_.asVec3Double("RangeZ").y; }

// Return z bin width
double Collect3DProcedureNode::zBinWidth() const { return keywords_.asVec3Double("RangeZ").z; }

/*
 * Branches
 */

// Add and return subcollection sequence branch
SequenceProcedureNode *Collect3DProcedureNode::addSubCollectBranch(ProcedureNode::NodeContext context)
{
    if (!subCollectBranch_)
        subCollectBranch_ = new SequenceProcedureNode(context, procedure());

    return subCollectBranch_;
}

// Return whether this node has a branch
bool Collect3DProcedureNode::hasBranch() const { return (subCollectBranch_ != nullptr); }
// Return SequenceNode for the branch (if it exists)
SequenceProcedureNode *Collect3DProcedureNode::branch() { return subCollectBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Collect3DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Construct our data name, and search for it in the supplied list
    std::string dataName = fmt::format("{}_{}_Bins", name(), cfg->niceName());
    bool created;
    auto &target =
        GenericListHelper<Histogram3D>::realise(targetList, dataName, prefix, GenericItem::InRestartFileFlag, &created);
    if (created)
    {
        Messenger::printVerbose("Three-dimensional histogram data for '{}' was not in the target list, so it will now "
                                "be initialised...\n",
                                name());
        target.initialise(xMinimum(), xMaximum(), xBinWidth(), yMinimum(), yMaximum(), yBinWidth(), zMinimum(), zMaximum(),
                          zBinWidth());
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
    std::tie(zObservable_, zObservableIndex_) = keywords_.retrieve<std::tuple<CalculateProcedureNodeBase *, int>>("QuantityZ");
    if (!zObservable_)
        return Messenger::error("No valid z quantity set in '{}'.\n", name());

    // Prepare any branches
    if (subCollectBranch_ && (!subCollectBranch_->prepare(cfg, prefix, targetList)))
        return false;

    return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult Collect3DProcedureNode::execute(ProcessPool &procPool, Configuration *cfg,
                                                                   std::string_view prefix, GenericList &targetList)
{
#ifdef CHECKS
    if (!xObservable_)
    {
        Messenger::error("No CalculateProcedureNodeBase pointer set for X observable in Collect3DProcedureNode '{}'.\n",
                         name());
        return ProcedureNode::Failure;
    }
    if (!yObservable_)
    {
        Messenger::error("No CalculateProcedureNodeBase pointer set for Y observable in Collect3DProcedureNode '{}'.\n",
                         name());
        return ProcedureNode::Failure;
    }
    if (!zObservable_)
    {
        Messenger::error("No CalculateProcedureNodeBase pointer set for Z observable in Collect3DProcedureNode '{}'.\n",
                         name());
        return ProcedureNode::Failure;
    }
#endif
    // Bin the current value of the observable
    if (histogram_->bin(xObservable_->value(xObservableIndex_), yObservable_->value(yObservableIndex_),
                        zObservable_->value(zObservableIndex_)) &&
        subCollectBranch_)
        return subCollectBranch_->execute(procPool, cfg, prefix, targetList);

    return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool Collect3DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                      GenericList &targetList)
{
#ifdef CHECKS
    if (!histogram_)
    {
        Messenger::error("No Data3D pointer set in Collect3DProcedureNode '{}'.\n", name());
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
