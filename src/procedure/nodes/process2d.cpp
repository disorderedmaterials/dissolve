/*
	*** Procedure Node - Process2D
	*** src/procedure/nodes/process2d.cpp
	Copyright T. Youngs 2012-2019

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

#include "procedure/nodes/process2d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/operatebase.h"
#include "keywords/types.h"
#include "io/export/data2d.h"
#include "math/integrator.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "genericitems/listhelper.h"

// Constructor
Process2DProcedureNode::Process2DProcedureNode(Collect2DProcedureNode* target) : ProcedureNode(ProcedureNode::Process2DNode)
{
	keywords_.add("Target", new NodeKeyword<Collect2DProcedureNode>(this, ProcedureNode::Collect2DNode, false, target), "SourceData", "Collect2D node containing the data to process");
	keywords_.add("Target", new CharStringKeyword("Counts"), "LabelValue", "Label for the value axis");
	keywords_.add("Target", new CharStringKeyword("X"), "LabelX", "Label for the x axis");
	keywords_.add("Target", new CharStringKeyword("Y"), "LabelY", "Label for the y axis");
	keywords_.add("Export", new BoolKeyword(false), "Save", "Save processed data to disk");
	keywords_.add("HIDDEN", new NodeBranchKeyword(this, &normalisationBranch_, ProcedureNode::OperateContext), "Normalisation", "Branch providing normalisation operations for the data");

	// Initialise branch
	normalisationBranch_ = NULL;

	// Initialise data pointer
	processedData_ = NULL;
}

// Destructor
Process2DProcedureNode::~Process2DProcedureNode()
{
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Process2DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
	return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return processed data
const Data2D& Process2DProcedureNode::processedData() const
{
	if (!processedData_)
	{
		Messenger::error("No processed data pointer set in Process2DProcedureNode, so nothing to return.\n");
		static Data2D dummy;
		return dummy;
	}

	return (*processedData_);
}

// Return value label
const char* Process2DProcedureNode::valueLabel() const
{
	return keywords_.asString("LabelValue");
}

// Return x axis label
const char* Process2DProcedureNode::xAxisLabel() const
{
	return keywords_.asString("LabelX");
}

// Return y axis label
const char* Process2DProcedureNode::yAxisLabel() const
{
	return keywords_.asString("LabelY");
}

/*
 * Branches
 */

// Add and return subcollection sequence branch
SequenceProcedureNode* Process2DProcedureNode::addNormalisationBranch()
{
	if (!normalisationBranch_) normalisationBranch_ = new SequenceProcedureNode(ProcedureNode::OperateContext, procedure());

	return normalisationBranch_;
}

// Return whether this node has a branch
bool Process2DProcedureNode::hasBranch() const
{
	return (normalisationBranch_ != NULL);
}

// Return SequenceNode for the branch (if it exists)
SequenceProcedureNode* Process2DProcedureNode::branch()
{
	return normalisationBranch_;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Process2DProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Retrieve the Collect2D node target
	collectNode_ = keywords_.retrieve<Collect2DProcedureNode*>("SourceData");
	if (!collectNode_) return Messenger::error("No source Collect2D node set in '%s'.\n", name());

	if (normalisationBranch_) normalisationBranch_->prepare(cfg, prefix, targetList);

	return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult Process2DProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool Process2DProcedureNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Retrieve / realise the normalised data from the supplied list
	bool created;
	Data2D& data = GenericListHelper<Data2D>::realise(targetList, CharString("%s_%s", name(), cfg->niceName()), prefix, GenericItem::InRestartFileFlag, &created);
	processedData_ = &data;

	data.setName(name());
	data.setObjectTag(CharString("%s//Process2D//%s//%s", prefix, cfg->name(), name()));

	// Copy the averaged data from the associated Collect2D node
	data = collectNode_->accumulatedData();

	// Run normalisation on the data
	if (normalisationBranch_)
	{
		// Set data targets in the normalisation nodes  TODO Will not work for sub-branches, if they are ever required
		ListIterator<ProcedureNode> nodeIterator(normalisationBranch_->sequence());
		while (ProcedureNode* node = nodeIterator.iterate())
		{
			if (!node->isType(ProcedureNode::OperateBaseNode)) continue;

			// Cast the node
			OperateProcedureNodeBase* operateNode = dynamic_cast<OperateProcedureNodeBase*>(node);
			operateNode->setTarget(processedData_);
		}

		ProcedureNode::NodeExecutionResult result = normalisationBranch_->execute(procPool, cfg, prefix, targetList);
		if (result != ProcedureNode::Success) return result;
	}

	// Save data?
	if (keywords_.asBool("Save"))
	{
		if (procPool.isMaster())
		{
			Data2DExportFileFormat data2DFormat(CharString("%s_%s.txt", name(), cfg->name()), Data2DExportFileFormat::CartesianData);
			if (data2DFormat.exportData(data)) procPool.decideTrue();
			else return procPool.decideFalse();
		}
		else if (!procPool.decision()) return false;
	}

	return true;
}
