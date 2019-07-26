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
#include "procedure/nodescopestack.h"
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
	collectNode_.addAllowableNodeType(ProcedureNode::Collect2DNode);

	collectNode_ = target;
	processedData_ = NULL;
	saveData_ = false;
	normalisationFactor_ = 0.0;
	normaliseByFactor_ = false;
	normaliseToOne_ = false;
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
 * Node Keywords
 */

// Return enum option info for Process2DNodeKeyword
EnumOptions<Process2DProcedureNode::Process2DNodeKeyword> Process2DProcedureNode::process2DNodeKeywords()
{
	static EnumOptionsList Process2DNodeTypeKeywords = EnumOptionsList() <<
		EnumOption(Process2DProcedureNode::EndProcess2DKeyword,			"EndProcess2D") <<
		EnumOption(Process2DProcedureNode::FactorKeyword,			"Factor",		1) <<
		EnumOption(Process2DProcedureNode::LabelValueKeyword,			"LabelValue",		1) <<
		EnumOption(Process2DProcedureNode::LabelXKeyword,			"LabelX",		1) <<
		EnumOption(Process2DProcedureNode::LabelYKeyword,			"LabelY",		1) <<
		EnumOption(Process2DProcedureNode::NormaliseToOneKeyword,		"NormaliseToOne",	1) <<
		EnumOption(Process2DProcedureNode::NSitesKeyword,			"NSites",		EnumOption::OneOrMoreArguments) <<
		EnumOption(Process2DProcedureNode::NumberDensityKeyword,		"NumberDensity",	EnumOption::OneOrMoreArguments) <<
		EnumOption(Process2DProcedureNode::SaveKeyword,				"Save",			1) <<
		EnumOption(Process2DProcedureNode::SourceDataKeyword,			"SourceData",		EnumOption::OptionalSecondArgument);

	static EnumOptions<Process2DProcedureNode::Process2DNodeKeyword> options("Process2DNodeKeyword", Process2DNodeTypeKeywords);

	return options;
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

// Add site population normaliser
void Process2DProcedureNode::addSitePopulationNormaliser(SelectProcedureNode* selectNode)
{
	sitePopulationNormalisers_.add(selectNode, 1.0);
}

// Add number density normaliser
void Process2DProcedureNode::addNumberDensityNormaliser(SelectProcedureNode* selectNode)
{
	numberDensityNormalisers_.add(selectNode);
}

// Set whether to normalise by factor
void Process2DProcedureNode::setNormaliseByFactor(bool on)
{
	normaliseByFactor_ = on;
}

// Set normalisation factor
void Process2DProcedureNode::setNormalisationFactor(double factor)
{
	normalisationFactor_ = factor;
}

// Set whether to normalise summed histogram value to 1.0
void Process2DProcedureNode::setNormaliseToOne(bool b)
{
	normaliseToOne_ = b;
}

// Return whether to normalise summed histogram value to 1.0
bool Process2DProcedureNode::normaliseToOne() const
{
	return normaliseToOne_;
}

// Set whether to save processed data
void Process2DProcedureNode::setSaveData(bool on)
{
	saveData_ = on;
}

// Set value label
void Process2DProcedureNode::setValueLabel(const char* label)
{
	valueLabel_ = label;
}

// Return value label
const char* Process2DProcedureNode::valueLabel() const
{
	return valueLabel_.get();
}

// Set x axis label
void Process2DProcedureNode::setXAxisLabel(const char* label)
{
	xAxisLabel_ = label;
}

// Return x axis label
const char* Process2DProcedureNode::xAxisLabel() const
{
	return xAxisLabel_.get();
}

// Set y axis label
void Process2DProcedureNode::setYAxisLabel(const char* label)
{
	yAxisLabel_ = label;
}

// Return y axis label
const char* Process2DProcedureNode::yAxisLabel() const
{
	return yAxisLabel_.get();
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Process2DProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
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

	data.setName(name());
	data.setObjectTag(CharString("%s//Process2D//%s//%s", prefix, cfg->name(), name()));
	processedData_ = &data;

	// Get the node pointer
	Collect2DProcedureNode* node = dynamic_cast<Collect2DProcedureNode*>(collectNode_.node());
	if (!node) return Messenger::error("No Collect2D node available in Process2DProcedureNode::finalise().\n");

	// Copy the averaged data from the associated Collect2D node, and normalise it accordingly
	data = node->accumulatedData();

	// Normalisation by number of sites?
	RefListIterator<SelectProcedureNode,double> siteNormaliserIterator(sitePopulationNormalisers_);
	while (SelectProcedureNode* selectNode = siteNormaliserIterator.iterate()) data /= selectNode->nAverageSites();

	// Normalisation by number density of sites?
	RefListIterator<SelectProcedureNode,double> numberDensityIterator(numberDensityNormalisers_);
	while (SelectProcedureNode* selectNode = numberDensityIterator.iterate()) data /= (selectNode->nAverageSites() / cfg->box()->volume());

	// Normalisation by factor?
	if (normaliseByFactor_) data /= normalisationFactor_;

	// Normalise to 1.0?
	if (normaliseToOne_)
	{
		// Get sum of absolute values
		double sum = Integrator::absSum(data);
		data /= sum;
	}

	// Save data?
	if (saveData_ && procPool.isMaster())
	{
		Data2DExportFileFormat data2DFormat(CharString("%s_%s.txt", name(), cfg->name()), Data2DExportFileFormat::CartesianData);
		if (data2DFormat.exportData(data)) procPool.decideTrue();
		else return procPool.decideFalse();
	}
	else if (!procPool.decision()) return false;

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool Process2DProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack)
{
	// The current line in the parser may contain a node name for us
	if (parser.nArgs() == 2) setName(parser.argc(1));

	// Add ourselves to the scope stack
	if (!scopeStack.add(this)) return Messenger::error("Error adding Process2D node '%s' to scope stack.\n", name());

	// Read until we encounter the EndProcess2D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim() != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (!process2DNodeKeywords().isValid(parser.argc(0))) return process2DNodeKeywords().errorAndPrintValid(parser.argc(0));
		Process2DNodeKeyword nk = process2DNodeKeywords().enumeration(parser.argc(0));
		if (!process2DNodeKeywords().validNArgs(nk, parser.nArgs()-1)) return false;

		// All OK, so process it
		switch (nk)
		{
			case (Process2DProcedureNode::EndProcess2DKeyword):
				// Check that a valid collectNode_ has been set
				if (collectNode_.isNull()) return Messenger::error("A valid Collect2D node must be set in the Process2D node '%s' using the '%s' keyword.\n", name(), process2DNodeKeywords().keyword(Process2DProcedureNode::SourceDataKeyword));
				return true;
			case (Process2DProcedureNode::FactorKeyword):
				normalisationFactor_ = parser.argd(1);
				normaliseByFactor_ = true;
				break;
			case (Process2DProcedureNode::LabelValueKeyword):
				valueLabel_ = parser.argc(1);
				break;
			case (Process2DProcedureNode::LabelXKeyword):
				xAxisLabel_ = parser.argc(1);
				break;
			case (Process2DProcedureNode::LabelYKeyword):
				yAxisLabel_ = parser.argc(1);
				break;
			case (Process2DProcedureNode::NormaliseToOneKeyword):
				normaliseToOne_ = true;
				break;
			case (Process2DProcedureNode::NSitesKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					SelectProcedureNode* selectNode = dynamic_cast<SelectProcedureNode*>(scopeStack.node(parser.argc(n), ProcedureNode::SelectNode));
					if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(n), process2DNodeKeywords().keyword(Process2DProcedureNode::NSitesKeyword));
					sitePopulationNormalisers_.add(selectNode, 1.0);
				}
				break;
			case (Process2DProcedureNode::NumberDensityKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					SelectProcedureNode* selectNode = dynamic_cast<SelectProcedureNode*>(scopeStack.node(parser.argc(n), ProcedureNode::SelectNode));
					if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(n), process2DNodeKeywords().keyword(Process2DProcedureNode::NumberDensityKeyword));
					numberDensityNormalisers_.add(selectNode, 1.0);
				}
				break;
			case (Process2DProcedureNode::SaveKeyword):
				saveData_ = parser.argb(1);
				break;
			case (Process2DProcedureNode::SourceDataKeyword):
				if (!collectNode_.read(parser, 1, coreData, scopeStack)) return Messenger::error("Couldn't set source data for node.\n");
				break;
			case (Process2DProcedureNode::nProcess2DNodeKeywords):
				return Messenger::error("Unrecognised Process2D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Process2D node keyword '%s'.\n", parser.argc(0));
		}
	}

	// Check that a valid collectNode_ has been set
	if (collectNode_.isNull()) return Messenger::error("A valid Collect2D node must be set in the Process2D node '%s' using the '%s' keyword.\n", name(), process2DNodeKeywords().keyword(Process2DProcedureNode::SourceDataKeyword));

	return true;
}

// Write structure to specified LineParser
bool Process2DProcedureNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
