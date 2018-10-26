/*
	*** Analysis Node - Process1D
	*** src/analyse/nodes/process1d.cpp
	Copyright T. Youngs 2012-2018

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

#include "analyse/nodes/process1d.h"
#include "analyse/nodes/collect1d.h"
#include "analyse/nodes/select.h"
#include "analyse/nodecontextstack.h"
#include "modules/analyse/analyse.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Constructor
AnalysisProcess1DNode::AnalysisProcess1DNode(AnalysisCollect1DNode* target) : AnalysisNode(AnalysisNode::Process1DNode)
{
	collectNode_ = target;
	saveData_ = false;
	normalisationFactor_ = 0.0;
	normaliseByFactor_ = false;
	normaliseBySphericalShellVolume_ = false;
}

// Destructor
AnalysisProcess1DNode::~AnalysisProcess1DNode()
{
}

/*
 * Node Keywords
 */

// Node Keywords (note ordering for efficiency)
const char* Process1DNodeKeywords[] = { "EndProcess1D", "Factor", "LabelValue", "LabelX", "NSites", "NumberDensity", "Save", "SphericalShellVolume" };

// Convert string to node keyword
AnalysisProcess1DNode::Process1DNodeKeyword AnalysisProcess1DNode::normalise1DNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisProcess1DNode::nProcess1DNodeKeywords; ++nk) if (DissolveSys::sameString(s, Process1DNodeKeywords[nk])) return (AnalysisProcess1DNode::Process1DNodeKeyword) nk;

	return AnalysisProcess1DNode::nProcess1DNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisProcess1DNode::normalise1DNodeKeyword(AnalysisProcess1DNode::Process1DNodeKeyword nk)
{
	return Process1DNodeKeywords[nk];
}

/*
 * Data
 */

// Add site population normaliser
void AnalysisProcess1DNode::addSitePopulationNormaliser(AnalysisSelectNode* selectNode)
{
	sitePopulationNormalisers_.add(selectNode, 1.0);
}

// Add number density normaliser
void AnalysisProcess1DNode::addNumberDensityNormaliser(AnalysisSelectNode* selectNode)
{
	numberDensityNormalisers_.add(selectNode);
}

// Set whether to normalise by factor
void AnalysisProcess1DNode::setNormaliseByFactor(bool on)
{
	normaliseByFactor_ = on;
}

// Set normalisation factor
void AnalysisProcess1DNode::setNormalisationFactor(double factor)
{
	normalisationFactor_ = factor;
}

// Set whether to normalise by spherical shell volume
void AnalysisProcess1DNode::setNormaliseBySphericalShellVolume(bool on)
{
	normaliseBySphericalShellVolume_ = on;
}

// Set whether to save processed data
void AnalysisProcess1DNode::setSaveData(bool on)
{
	saveData_ = on;
}

// Set value label
void AnalysisProcess1DNode::setValueLabel(const char* label)
{
	valueLabel_ = label;
}

// Return value label
const char* AnalysisProcess1DNode::valueLabel() const
{
	return valueLabel_.get();
}

// Set x axis label
void AnalysisProcess1DNode::setXAxisLabel(const char* label)
{
	xAxisLabel_ = label;
}

// Return x axis label
const char* AnalysisProcess1DNode::xAxisLabel() const
{
	return xAxisLabel_.get();
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AnalysisProcess1DNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return true;
}

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisProcess1DNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return AnalysisNode::Success;
}

// Finalise any necessary data after execution
bool AnalysisProcess1DNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Retrieve / realise the normalised data from the supplied list
	bool created;
	Data1D& data = GenericListHelper<Data1D>::realise(targetList, CharString("%s_%s", name(), cfg->niceName()), prefix, GenericItem::InRestartFileFlag, &created);

	data.setName(name());
	data.setObjectTag(CharString("%s//Process1D//%s//%s", prefix, cfg->name(), name()));

	// Copy the averaged data from the associated Collect1D node, and normalise it accordingly
	data = collectNode_->accumulatedData();

	// Normalisation by number of sites?
	RefListIterator<AnalysisSelectNode,double> siteNormaliserIterator(sitePopulationNormalisers_);
	while (AnalysisSelectNode* selectNode = siteNormaliserIterator.iterate()) data /= selectNode->nAverageSites();

	// Normalisation by spherical shell?
	if (normaliseBySphericalShellVolume_)
	{
		double halfBinWidth = collectNode_->binWidth() * 0.5;
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
	RefListIterator<AnalysisSelectNode,double> numberDensityIterator(numberDensityNormalisers_);
	while (AnalysisSelectNode* selectNode = numberDensityIterator.iterate()) data /= (selectNode->nAverageSites() / cfg->box()->volume());

	// Normalisation by factor?
	if (normaliseByFactor_) data /= normalisationFactor_;

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
bool AnalysisProcess1DNode::read(LineParser& parser, NodeContextStack& contextStack)
{
	// The current line in the parser must also contain the name of a Collect1D node which we will operate on (it will also become our node name)
	if (parser.nArgs() < 2) return Messenger::error("A Process1D node must be given the name of a Collect1D node.\n");

	// If a second argument was provided we assume this is the name of an AnalyseModule
	AnalyseModule* analyseModule = NULL;
	if (parser.nArgs() == 3)
	{
		Module* module = ModuleList::findInstanceByUniqueName(parser.argc(2));
		if (!module) return Messenger::error("No Analyse module named '%s' exists.\n", parser.argc(2));
		if (!DissolveSys::sameString("Analyse", module->type())) return Messenger::error("Specified module '%s' must be an Analyse module.\n", parser.argc(2));

		// Found the target AnalyseModule, so cast it up and search for the named Collect1D data in its Analyser
		analyseModule = (AnalyseModule*) module;
		collectNode_ = analyseModule->analyserContextStack().collect1DNode(parser.argc(1));
	}
	else collectNode_ = contextStack.collect1DNode(parser.argc(1));
	if (!collectNode_) return Messenger::error("A valid Collect1D node name must be given as an argument to Process1D.\n");
	setName(parser.argc(1));

	// Set the target context stack to search (it may not be the one passed...)
	const NodeContextStack& targetStack = analyseModule ? analyseModule->analyserContextStack() : contextStack;
	AnalysisSelectNode* selectNode;

	// Read until we encounter the EndProcess1D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		Process1DNodeKeyword nk = normalise1DNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (Process1DNodeKeyword::EndProcess1DKeyword):
				return true;
			case (Process1DNodeKeyword::FactorKeyword):
				normalisationFactor_ = parser.argd(1);
				normaliseByFactor_ = true;
				break;
			case (Process1DNodeKeyword::LabelValueKeyword):
				valueLabel_ = parser.argc(1);
				break;
			case (Process1DNodeKeyword::LabelXKeyword):
				xAxisLabel_ = parser.argc(1);
				break;
			case (Process1DNodeKeyword::NSitesKeyword):
				selectNode = contextStack.selectNode(parser.argc(1));
				if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(0), normalise1DNodeKeyword(Process1DNodeKeyword::NSitesKeyword));
				sitePopulationNormalisers_.add(selectNode, 1.0);
				break;
			case (Process1DNodeKeyword::NumberDensityKeyword):
				selectNode = contextStack.selectNode(parser.argc(1));
				if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(0), normalise1DNodeKeyword(Process1DNodeKeyword::NumberDensityKeyword));
				numberDensityNormalisers_.add(selectNode, 1.0);
				break;
			case (Process1DNodeKeyword::SaveKeyword):
				saveData_ = parser.argb(1);
				break;
			case (Process1DNodeKeyword::SphericalShellVolumeKeyword):
				normaliseBySphericalShellVolume_ = parser.argb(1);
				break;
			case (Process1DNodeKeyword::nProcess1DNodeKeywords):
				return Messenger::error("Unrecognised Process1D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Process1D node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisProcess1DNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
