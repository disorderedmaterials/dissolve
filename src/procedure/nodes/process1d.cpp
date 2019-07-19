/*
	*** Procedure Node - Process1D
	*** src/procedure/nodes/process1d.cpp
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

#include "procedure/nodes/process1d.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/select.h"
#include "procedure/nodescopestack.h"
#include "modules/analyse/analyse.h"
#include "math/integrator.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "genericitems/listhelper.h"

// Constructor
Process1DProcedureNode::Process1DProcedureNode(Collect1DProcedureNode* target) : ProcedureNode(ProcedureNode::Process1DNode)
{
	collectNode_.addAllowableNodeType(ProcedureNode::Collect1DNode);

	collectNode_ = target;
	processedData_ = NULL;
	saveData_ = false;
	normalisationFactor_ = 0.0;
	normaliseByFactor_ = false;
	normaliseBySphericalShellVolume_ = false;
	normaliseToOne_ = false;
}

// Destructor
Process1DProcedureNode::~Process1DProcedureNode()
{
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Process1DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
	return (context == ProcedureNode::AnalysisContext);
}

/*
 * Node Keywords
 */

// Return enum option info for Process1DNodeKeyword
EnumOptions<Process1DProcedureNode::Process1DNodeKeyword> Process1DProcedureNode::process1DNodeKeywords()
{
	static EnumOptionsList Process1DNodeTypeKeywords = EnumOptionsList() <<
		EnumOption(Process1DProcedureNode::EndProcess1DKeyword,			"EndProcess1D") <<
		EnumOption(Process1DProcedureNode::FactorKeyword,			"Factor") <<
		EnumOption(Process1DProcedureNode::LabelValueKeyword,			"LabelValue") <<
		EnumOption(Process1DProcedureNode::LabelXKeyword,			"LabelX") <<
		EnumOption(Process1DProcedureNode::NormaliseToOneKeyword,		"NormaliseToOne") <<
		EnumOption(Process1DProcedureNode::NSitesKeyword,			"NSites") <<
		EnumOption(Process1DProcedureNode::NumberDensityKeyword,		"NumberDensity") <<
		EnumOption(Process1DProcedureNode::SaveKeyword,				"Save") <<
		EnumOption(Process1DProcedureNode::SourceDataKeyword,			"SourceData") <<
		EnumOption(Process1DProcedureNode::SphericalShellVolumeKeyword,		"SphericalShellVolume");

	static EnumOptions<Process1DProcedureNode::Process1DNodeKeyword> options("Process1DNodeKeyword", Process1DNodeTypeKeywords);

	return options;
}

/*
 * Data
 */

// Return processed data
const Data1D& Process1DProcedureNode::processedData() const
{
	if (!processedData_)
	{
		Messenger::error("No processed data pointer set in Process1DProcedureNode, so nothing to return.\n");
		static Data1D dummy;
		return dummy;
	}

	return (*processedData_);
}

// Add site population normaliser
void Process1DProcedureNode::addSitePopulationNormaliser(SelectProcedureNode* selectNode)
{
	sitePopulationNormalisers_.add(selectNode, 1.0);
}

// Add number density normaliser
void Process1DProcedureNode::addNumberDensityNormaliser(SelectProcedureNode* selectNode)
{
	numberDensityNormalisers_.add(selectNode);
}

// Set whether to normalise by factor
void Process1DProcedureNode::setNormaliseByFactor(bool on)
{
	normaliseByFactor_ = on;
}

// Set normalisation factor
void Process1DProcedureNode::setNormalisationFactor(double factor)
{
	normalisationFactor_ = factor;
}

// Set whether to normalise by spherical shell volume
void Process1DProcedureNode::setNormaliseBySphericalShellVolume(bool on)
{
	normaliseBySphericalShellVolume_ = on;
}

// Set whether to normalise to one
void Process1DProcedureNode::setNormaliseToOne(bool on)
{
	normaliseToOne_ = on;
}

// Set whether to save processed data
void Process1DProcedureNode::setSaveData(bool on)
{
	saveData_ = on;
}

// Set value label
void Process1DProcedureNode::setValueLabel(const char* label)
{
	valueLabel_ = label;
}

// Return value label
const char* Process1DProcedureNode::valueLabel() const
{
	return valueLabel_.get();
}

// Set x axis label
void Process1DProcedureNode::setXAxisLabel(const char* label)
{
	xAxisLabel_ = label;
}

// Return x axis label
const char* Process1DProcedureNode::xAxisLabel() const
{
	return xAxisLabel_.get();
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Process1DProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult Process1DProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool Process1DProcedureNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Retrieve / realise the normalised data from the supplied list
	bool created;
	Data1D& data = GenericListHelper<Data1D>::realise(targetList, CharString("%s_%s", name(), cfg->niceName()), prefix, GenericItem::InRestartFileFlag, &created);
	processedData_ = &data;

	data.setName(name());
	data.setObjectTag(CharString("%s//Process1D//%s//%s", prefix, cfg->name(), name()));

	// Get the node pointer
	Collect1DProcedureNode* node = dynamic_cast<Collect1DProcedureNode*>(collectNode_.node());
	if (!node) return Messenger::error("No Collect1D node available in Process1DProcedureNode::finalise().\n");

	// Copy the averaged data from the associated Collect1D node, and normalise it accordingly
	data = node->accumulatedData();

	// Normalisation by number of sites?
	RefListIterator<SelectProcedureNode,double> siteNormaliserIterator(sitePopulationNormalisers_);
	while (SelectProcedureNode* selectNode = siteNormaliserIterator.iterate()) data /= selectNode->nAverageSites();

	// Normalisation by spherical shell?
	if (normaliseBySphericalShellVolume_)
	{
		double halfBinWidth = node->binWidth() * 0.5;
		double r1Cubed = pow(data.xAxis(0)-halfBinWidth,3), r2Cubed;
		for (int n = 0; n < data.nValues(); ++n)
		{
			r2Cubed = pow(data.xAxis(n)+halfBinWidth,3);
			data.value(n) /= (4.0/3.0) * PI * (r2Cubed - r1Cubed);
			data.error(n) /= (4.0/3.0) * PI * (r2Cubed - r1Cubed);
			r1Cubed = r2Cubed;
		}
	}

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
		if (data.save(CharString("%s_%s.txt", name(), cfg->name()))) procPool.decideTrue();
		else return procPool.decideFalse();
	}
	else if (!procPool.decision()) return false;

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool Process1DProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack)
{
	// The current line in the parser may contain a node name for us
	if (parser.nArgs() == 2) setName(parser.argc(1));

	// Add ourselves to the scope stack
	if (!scopeStack.add(this)) return Messenger::error("Error adding Process1D node '%s' to scope stack.\n", name());

	// Read until we encounter the EndProcess1D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		Process1DNodeKeyword nk = process1DNodeKeywords().enumeration(parser.argc(0));
		switch (nk)
		{
			case (Process1DProcedureNode::EndProcess1DKeyword):
				return true;
			case (Process1DProcedureNode::FactorKeyword):
				normalisationFactor_ = parser.argd(1);
				normaliseByFactor_ = true;
				break;
			case (Process1DProcedureNode::LabelValueKeyword):
				valueLabel_ = parser.argc(1);
				break;
			case (Process1DProcedureNode::LabelXKeyword):
				xAxisLabel_ = parser.argc(1);
				break;
			case (Process1DProcedureNode::NSitesKeyword):
				// Need a valid collectNode_...
				if (collectNode_.isNull()) return Messenger::error("Can't set site-dependent normalisers without first setting the collect node target.\n");

				for (int n=1; n<parser.nArgs(); ++n)
				{
					SelectProcedureNode* selectNode = dynamic_cast<SelectProcedureNode*>(scopeStack.node(parser.argc(n), ProcedureNode::SelectNode));
					if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(n), process1DNodeKeywords().keyword(Process1DProcedureNode::NSitesKeyword));
					sitePopulationNormalisers_.add(selectNode, 1.0);
				}
				break;
			case (Process1DProcedureNode::NumberDensityKeyword):
				// Need a valid collectNode_...
				if (collectNode_.isNull()) return Messenger::error("Can't set site-dependent normalisers without first setting the collect node target.\n");

				for (int n=1; n<parser.nArgs(); ++n)
				{
					SelectProcedureNode* selectNode = dynamic_cast<SelectProcedureNode*>(scopeStack.node(parser.argc(n), ProcedureNode::SelectNode));
					if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(n), process1DNodeKeywords().keyword(Process1DProcedureNode::NumberDensityKeyword));
					numberDensityNormalisers_.add(selectNode, 1.0);
				}
				break;
			case (Process1DProcedureNode::NormaliseToOneKeyword):
				normaliseToOne_ = true;
				break;
			case (Process1DProcedureNode::SaveKeyword):
				saveData_ = parser.argb(1);
				break;
			case (Process1DProcedureNode::SourceDataKeyword):
				if (!collectNode_.read(parser, 1, coreData, scopeStack)) return Messenger::error("Couldn't set source data for node.\n");
				break;
			case (Process1DProcedureNode::SphericalShellVolumeKeyword):
				normaliseBySphericalShellVolume_ = parser.argb(1);
				break;
			case (Process1DProcedureNode::nProcess1DNodeKeywords):
				return Messenger::error("Unrecognised Process1D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Process1D node keyword '%s'.\n", parser.argc(0));
		}
	}

	// Check that a valid collectNode_ has been set
	if (collectNode_.isNull()) return Messenger::error("A valid Collect1D node must be set in the Process1D node '%s' using the '%s' keyword.\n", name(), process1DNodeKeywords().keyword(Process1DProcedureNode::SourceDataKeyword));

	return true;
}

// Write structure to specified LineParser
bool Process1DProcedureNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
