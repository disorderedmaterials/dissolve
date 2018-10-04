/*
	*** Analysis Node - Process2D
	*** src/analyse/nodes/process2d.cpp
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

#include "analyse/nodes/process2d.h"
#include "analyse/nodes/collect2d.h"
#include "analyse/nodes/select.h"
#include "analyse/nodecontextstack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Constructor
AnalysisProcess2DNode::AnalysisProcess2DNode(AnalysisCollect2DNode* target) : AnalysisNode(AnalysisNode::Process2DNode)
{
	collectNode_ = target;
	saveNormalisedData_ = false;
	normalisationFactor_ = 0.0;
	normaliseByFactor_ = false;
	normaliseToOne_ = false;
}

// Destructor
AnalysisProcess2DNode::~AnalysisProcess2DNode()
{
}

/*
 * Node Keywords
 */

// Node Keywords (note ordering for efficiency)
const char* Process2DNodeKeywords[] = { "EndProcess2D", "Factor", "LabelValue", "LabelX", "LabelY", "NormaliseToOne", "NSites", "NumberDensity", "Save" };

// Convert string to node keyword
AnalysisProcess2DNode::Process2DNodeKeyword AnalysisProcess2DNode::normalise2DNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisProcess2DNode::nProcess2DNodeKeywords; ++nk) if (DissolveSys::sameString(s, Process2DNodeKeywords[nk])) return (AnalysisProcess2DNode::Process2DNodeKeyword) nk;

	return AnalysisProcess2DNode::nProcess2DNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisProcess2DNode::normalise2DNodeKeyword(AnalysisProcess2DNode::Process2DNodeKeyword nk)
{
	return Process2DNodeKeywords[nk];
}

/*
 * Data
 */

// Add site population normaliser
void AnalysisProcess2DNode::addSitePopulationNormaliser(AnalysisSelectBaseNode* selectNode)
{
	sitePopulationNormalisers_.add(selectNode, 1.0);
}

// Add number density normaliser
void AnalysisProcess2DNode::addNumberDensityNormaliser(AnalysisSelectBaseNode* selectNode)
{
	numberDensityNormalisers_.add(selectNode);
}

// Set whether to normalise by factor
void AnalysisProcess2DNode::setNormaliseByFactor(bool on)
{
	normaliseByFactor_ = on;
}

// Set normalisation factor
void AnalysisProcess2DNode::setNormalisationFactor(double factor)
{
	normalisationFactor_ = factor;
}

// Set whether to normalise summed histogram value to 1.0
void AnalysisProcess2DNode::setNormaliseToOne(bool b)
{
	normaliseToOne_ = b;
}

// Return whether to normalise summed histogram value to 1.0
bool AnalysisProcess2DNode::normaliseToOne() const
{
	return normaliseToOne_;
}

// Set whether to save normalised data
void AnalysisProcess2DNode::setSaveNormalisedData(bool on)
{
	saveNormalisedData_ = on;
}

// Set value label
void AnalysisProcess2DNode::setValueLabel(const char* label)
{
	valueLabel_ = label;
}

// Return value label
const char* AnalysisProcess2DNode::valueLabel() const
{
	return valueLabel_.get();
}

// Set x axis label
void AnalysisProcess2DNode::setXAxisLabel(const char* label)
{
	xAxisLabel_ = label;
}

// Return x axis label
const char* AnalysisProcess2DNode::xAxisLabel() const
{
	return xAxisLabel_.get();
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AnalysisProcess2DNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return true;
}

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisProcess2DNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return AnalysisNode::Success;
}

// Finalise any necessary data after execution
bool AnalysisProcess2DNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Retrieve / realise the normalised data from the supplied listAnalysisProcess2DNod
	bool created;
	Data2D& normalisedData = GenericListHelper<Data2D>::realise(targetList, CharString("%s_%s", name(), cfg->niceName()), prefix, GenericItem::InRestartFileFlag, &created);

	normalisedData.setName(name());
	normalisedData.setObjectTag(CharString("%s//Process2D//%s//%s", prefix, cfg->name(), name()));

	// Copy the averaged data from the associated Collect2D node, and normalise it accordingly
	normalisedData = collectNode_->accumulatedData();

	// Normalisation by number of sites?
	RefListIterator<AnalysisSelectBaseNode,double> siteNormaliserIterator(sitePopulationNormalisers_);
	while (AnalysisSelectBaseNode* selectNode = siteNormaliserIterator.iterate()) normalisedData /= selectNode->nAverageSites();

	// Normalisation by number density of sites?
	RefListIterator<AnalysisSelectBaseNode,double> numberDensityIterator(numberDensityNormalisers_);
	while (AnalysisSelectBaseNode* selectNode = numberDensityIterator.iterate()) normalisedData /= (selectNode->nAverageSites() / cfg->box()->volume());

	// Normalisation by factor?
	if (normaliseByFactor_) normalisedData /= normalisationFactor_;

	// Save data?
	if (saveNormalisedData_ && procPool.isMaster())
	{
		return Messenger::error("Saving of 2D data is not yet implemented.\n");
// 		if (normalisedData.save(CharString("%s_%s.txt", name(), cfg->name()))) procPool.decideTrue();
// 		else return procPool.decideFalse();
	}
	else if (!procPool.decision()) return false;

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisProcess2DNode::read(LineParser& parser, NodeContextStack& contextStack)
{
	// The current line in the parser must also contain the name of a Collect2D node which we will operate on (it will also become our node name)
	if (parser.nArgs() != 2) return Messenger::error("A Process2D node must be given the name of a Collect2D node.\n");
	collectNode_ = contextStack.collect2DNode(parser.argc(1));
	if (!collectNode_) return Messenger::error("A valid Collect2D node name must be given as an argument to Process2D.\n");
	setName(parser.argc(1));

	AnalysisSelectBaseNode* selectNode;

	// Read until we encounter the EndProcess2D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		Process2DNodeKeyword nk = normalise2DNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (Process2DNodeKeyword::EndProcess2DKeyword):
				return true;
			case (Process2DNodeKeyword::FactorKeyword):
				normalisationFactor_ = parser.argd(1);
				normaliseByFactor_ = true;
				break;
			case (Process2DNodeKeyword::LabelValueKeyword):
				valueLabel_ = parser.argc(1);
				break;
			case (Process2DNodeKeyword::LabelXKeyword):
				xAxisLabel_ = parser.argc(1);
				break;
			case (Process2DNodeKeyword::LabelYKeyword):
				yAxisLabel_ = parser.argc(1);
				break;
			case (Process2DNodeKeyword::NormaliseToOneKeyword):
				normaliseToOne_ = parser.argb(1);
				break;
			case (Process2DNodeKeyword::NSitesKeyword):
				selectNode = contextStack.selectNode(parser.argc(1));
				if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(0), normalise2DNodeKeyword(Process2DNodeKeyword::NSitesKeyword));
				sitePopulationNormalisers_.add(selectNode, 1.0);
				break;
			case (Process2DNodeKeyword::NumberDensityKeyword):
				selectNode = contextStack.selectNode(parser.argc(1));
				if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(0), normalise2DNodeKeyword(Process2DNodeKeyword::NumberDensityKeyword));
				numberDensityNormalisers_.add(selectNode, 1.0);
				break;
			case (Process2DNodeKeyword::SaveKeyword):
				saveNormalisedData_ = parser.argb(1);
				break;
			case (Process2DNodeKeyword::nProcess2DNodeKeywords):
				return Messenger::error("Unrecognised Process2D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Process2D node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisProcess2DNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
