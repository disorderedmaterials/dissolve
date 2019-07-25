/*
	*** Procedure Node - Process3D
	*** src/procedure/nodes/process3d.cpp
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

#include "procedure/nodes/process3d.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/select.h"
#include "procedure/nodescopestack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "genericitems/listhelper.h"

// Constructor
Process3DProcedureNode::Process3DProcedureNode(Collect3DProcedureNode* target) : ProcedureNode(ProcedureNode::Process3DNode)
{
	collectNode_.addAllowableNodeType(ProcedureNode::Collect3DNode);

	collectNode_ = target;
	processedData_ = NULL;
	saveData_ = false;
	normalisationFactor_ = 0.0;
	normaliseByFactor_ = false;
}

// Destructor
Process3DProcedureNode::~Process3DProcedureNode()
{
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Process3DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
	return (context == ProcedureNode::AnalysisContext);
}

/*
 * Node Keywords
 */

// Return enum option info for Process3DNodeKeyword
EnumOptions<Process3DProcedureNode::Process3DNodeKeyword> Process3DProcedureNode::process3DNodeKeywords()
{
	static EnumOptionsList Process3DNodeTypeKeywords = EnumOptionsList() <<
		EnumOption(Process3DProcedureNode::EndProcess3DKeyword,			"EndProcess3D") <<
		EnumOption(Process3DProcedureNode::FactorKeyword,			"Factor",		1) <<
		EnumOption(Process3DProcedureNode::LabelValueKeyword,			"LabelValue",		1) <<
		EnumOption(Process3DProcedureNode::LabelXKeyword,			"LabelX",		1) <<
		EnumOption(Process3DProcedureNode::LabelYKeyword,			"LabelY",		1) <<
		EnumOption(Process3DProcedureNode::LabelZKeyword,			"LabelZ",		1) <<
		EnumOption(Process3DProcedureNode::NSitesKeyword,			"NSites",		EnumOption::OneOrMoreArguments) <<
		EnumOption(Process3DProcedureNode::NumberDensityKeyword,		"NumberDensity",	EnumOption::OneOrMoreArguments) <<
		EnumOption(Process3DProcedureNode::SaveKeyword,				"Save",			1) <<
		EnumOption(Process3DProcedureNode::SourceDataKeyword,			"SourceData",		EnumOption::OptionalSecondArgument);

	static EnumOptions<Process3DProcedureNode::Process3DNodeKeyword> options("Process3DNodeKeyword", Process3DNodeTypeKeywords);

	return options;
}

/*
 * Data
 */

// Return processed data
const Data3D& Process3DProcedureNode::processedData() const
{
	if (!processedData_)
	{
		Messenger::error("No processed data pointer set in Process3DProcedureNode, so nothing to return.\n");
		static Data3D dummy;
		return dummy;
	}

	return (*processedData_);
}

// Add site population normaliser
void Process3DProcedureNode::addSitePopulationNormaliser(SelectProcedureNode* selectNode)
{
	sitePopulationNormalisers_.add(selectNode, 1.0);
}

// Add number density normaliser
void Process3DProcedureNode::addNumberDensityNormaliser(SelectProcedureNode* selectNode)
{
	numberDensityNormalisers_.add(selectNode);
}

// Set whether to normalise by factor
void Process3DProcedureNode::setNormaliseByFactor(bool on)
{
	normaliseByFactor_ = on;
}

// Set normalisation factor
void Process3DProcedureNode::setNormalisationFactor(double factor)
{
	normalisationFactor_ = factor;
}

// Set whether to save processed data
void Process3DProcedureNode::setSaveData(bool on)
{
	saveData_ = on;
}

// Set value label
void Process3DProcedureNode::setValueLabel(const char* label)
{
	valueLabel_ = label;
}

// Return value label
const char* Process3DProcedureNode::valueLabel() const
{
	return valueLabel_.get();
}

// Set x axis label
void Process3DProcedureNode::setXAxisLabel(const char* label)
{
	xAxisLabel_ = label;
}

// Return x axis label
const char* Process3DProcedureNode::xAxisLabel() const
{
	return xAxisLabel_.get();
}

// Set y axis label
void Process3DProcedureNode::setYAxisLabel(const char* label)
{
	yAxisLabel_ = label;
}

// Return y axis label
const char* Process3DProcedureNode::yAxisLabel() const
{
	return yAxisLabel_.get();
}

// Set z axis label
void Process3DProcedureNode::setZAxisLabel(const char* label)
{
	zAxisLabel_ = label;
}

// Return z axis label
const char* Process3DProcedureNode::zAxisLabel() const
{
	return zAxisLabel_.get();
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Process3DProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult Process3DProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool Process3DProcedureNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Retrieve / realise the normalised data from the supplied list
	bool created;
	Data3D& data = GenericListHelper<Data3D>::realise(targetList, CharString("%s_%s", name(), cfg->niceName()), prefix, GenericItem::InRestartFileFlag, &created);
	processedData_ = &data;

	data.setName(name());
	data.setObjectTag(CharString("%s//Process3D//%s//%s", prefix, cfg->name(), name()));

	// Get the node pointer
	Collect3DProcedureNode* node = dynamic_cast<Collect3DProcedureNode*>(collectNode_.node());
	if (!node) return Messenger::error("No Collect3D node available in Process3DProcedureNode::finalise().\n");

	// Copy the averaged data from the associated Collect3D node, and normalise it accordingly
	data = node->accumulatedData();

	// Normalisation by number of sites?
	RefListIterator<SelectProcedureNode,double> siteNormaliserIterator(sitePopulationNormalisers_);
	while (SelectProcedureNode* selectNode = siteNormaliserIterator.iterate()) data /= selectNode->nAverageSites();

	// Normalisation by number density of sites?
	RefListIterator<SelectProcedureNode,double> numberDensityIterator(numberDensityNormalisers_);
	while (SelectProcedureNode* selectNode = numberDensityIterator.iterate()) data /= (selectNode->nAverageSites() / cfg->box()->volume());

	// Normalisation by factor?
	if (normaliseByFactor_) data /= normalisationFactor_;

	// Save data?
	if (saveData_ && procPool.isMaster())
	{
		return Messenger::error("Saving of 3D data is not yet implemented.\n");
// 		if (data.save(CharString("%s_%s.txt", name(), cfg->name()))) procPool.decideTrue();
// 		else return procPool.decideFalse();
	}
	else if (!procPool.decision()) return false;

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool Process3DProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack)
{
	// The current line in the parser may contain a node name for us
	if (parser.nArgs() == 2) setName(parser.argc(1));

	// Add ourselves to the scope stack
	if (!scopeStack.add(this)) return Messenger::error("Error adding Process3D node '%s' to scope stack.\n", name());

	// Read until we encounter the EndProcess3D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (!process3DNodeKeywords().isValid(parser.argc(0))) return process3DNodeKeywords().errorAndPrintValid(parser.argc(0));
		Process3DNodeKeyword nk = process3DNodeKeywords().enumeration(parser.argc(0));
		if (!process3DNodeKeywords().validNArgs(nk, parser.nArgs()-1)) return false;

		// All OK, so process it
		switch (nk)
		{
			case (Process3DProcedureNode::EndProcess3DKeyword):
				return true;
			case (Process3DProcedureNode::FactorKeyword):
				normalisationFactor_ = parser.argd(1);
				normaliseByFactor_ = true;
				break;
			case (Process3DProcedureNode::LabelValueKeyword):
				valueLabel_ = parser.argc(1);
				break;
			case (Process3DProcedureNode::LabelXKeyword):
				xAxisLabel_ = parser.argc(1);
				break;
			case (Process3DProcedureNode::LabelYKeyword):
				yAxisLabel_ = parser.argc(1);
				break;
			case (Process3DProcedureNode::LabelZKeyword):
				zAxisLabel_ = parser.argc(1);
				break;
			case (Process3DProcedureNode::NSitesKeyword):
				// Need a valid collectNode_...
				if (collectNode_.isNull()) return Messenger::error("Can't set site-dependent normalisers without first setting the CollectNode target.\n");

				for (int n=1; n<parser.nArgs(); ++n)
				{
					SelectProcedureNode* selectNode = dynamic_cast<SelectProcedureNode*>(scopeStack.node(parser.argc(n), ProcedureNode::SelectNode));
					if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(n), process3DNodeKeywords().keyword(Process3DProcedureNode::NSitesKeyword));
					sitePopulationNormalisers_.add(selectNode, 1.0);
				}
				break;
			case (Process3DProcedureNode::NumberDensityKeyword):
				// Need a valid collectNode_...
				if (collectNode_.isNull()) return Messenger::error("Can't set site-dependent normalisers without first setting the collect node target.\n");

				for (int n=1; n<parser.nArgs(); ++n)
				{
					SelectProcedureNode* selectNode = dynamic_cast<SelectProcedureNode*>(scopeStack.node(parser.argc(n), ProcedureNode::SelectNode));
					if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(n), process3DNodeKeywords().keyword(Process3DProcedureNode::NumberDensityKeyword));
					numberDensityNormalisers_.add(selectNode, 1.0);
				}
				break;
			case (Process3DProcedureNode::SaveKeyword):
				saveData_ = parser.argb(1);
				break;
			case (Process3DProcedureNode::SourceDataKeyword):
				if (!collectNode_.read(parser, 1, coreData ,scopeStack)) return Messenger::error("Couldn't set source data for node.\n");
				break;
			case (Process3DProcedureNode::nProcess3DNodeKeywords):
				return Messenger::error("Unrecognised Process3D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Process3D node keyword '%s'.\n", parser.argc(0));
		}
	}

	// Check that a valid collectNode_ has been set
	if (collectNode_.isNull()) return Messenger::error("A valid Collect3D node must be set in the Process3D node '%s' using the '%s' keyword.\n", name(), process3DNodeKeywords().keyword(Process3DProcedureNode::SourceDataKeyword));

	return true;
}

// Write structure to specified LineParser
bool Process3DProcedureNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
