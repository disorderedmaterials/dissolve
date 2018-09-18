/*
	*** Analysis Node - Normalise3D
	*** src/analyse/nodes/normalise3d.cpp
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

#include "analyse/nodes/normalise3d.h"
#include "analyse/nodes/collect3d.h"
#include "analyse/nodes/select.h"
#include "analyse/nodecontextstack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Constructor
AnalysisNormalise3DNode::AnalysisNormalise3DNode(AnalysisCollect3DNode* target) : AnalysisNode(AnalysisNode::Normalise3DNode)
{
	collectNode_ = target;
	saveNormalisedData_ = false;
	normalisationFactor_ = 0.0;
	normaliseByFactor_ = false;
}

// Destructor
AnalysisNormalise3DNode::~AnalysisNormalise3DNode()
{
}

/*
 * Node Keywords
 */

// Node Keywords (note ordering for efficiency)
const char* Normalise3DNodeKeywords[] = { "EndNormalise3D", "Factor", "LabelValue", "LabelX", "LabelY", "LabelZ", "NSites", "NumberDensity", "Save" };

// Convert string to node keyword
AnalysisNormalise3DNode::Normalise3DNodeKeyword AnalysisNormalise3DNode::normalise3DNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisNormalise3DNode::nNormalise3DNodeKeywords; ++nk) if (DissolveSys::sameString(s, Normalise3DNodeKeywords[nk])) return (AnalysisNormalise3DNode::Normalise3DNodeKeyword) nk;

	return AnalysisNormalise3DNode::nNormalise3DNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisNormalise3DNode::normalise3DNodeKeyword(AnalysisNormalise3DNode::Normalise3DNodeKeyword nk)
{
	return Normalise3DNodeKeywords[nk];
}

/*
 * Data
 */

// Add site population normaliser
void AnalysisNormalise3DNode::addSitePopulationNormaliser(AnalysisSelectBaseNode* selectNode)
{
	sitePopulationNormalisers_.add(selectNode, 1.0);
}

// Add number density normaliser
void AnalysisNormalise3DNode::addNumberDensityNormaliser(AnalysisSelectBaseNode* selectNode)
{
	numberDensityNormalisers_.add(selectNode);
}

// Set whether to normalise by factor
void AnalysisNormalise3DNode::setNormaliseByFactor(bool on)
{
	normaliseByFactor_ = on;
}

// Set normalisation factor
void AnalysisNormalise3DNode::setNormalisationFactor(double factor)
{
	normalisationFactor_ = factor;
}

// Set whether to save normalised data
void AnalysisNormalise3DNode::setSaveNormalisedData(bool on)
{
	saveNormalisedData_ = on;
}

// Set value label
void AnalysisNormalise3DNode::setValueLabel(const char* label)
{
	valueLabel_ = label;
}

// Return value label
const char* AnalysisNormalise3DNode::valueLabel() const
{
	return valueLabel_.get();
}

// Set x axis label
void AnalysisNormalise3DNode::setXAxisLabel(const char* label)
{
	xAxisLabel_ = label;
}

// Return x axis label
const char* AnalysisNormalise3DNode::xAxisLabel() const
{
	return xAxisLabel_.get();
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AnalysisNormalise3DNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return true;
}

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisNormalise3DNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return AnalysisNode::Success;
}

// Finalise any necessary data after execution
bool AnalysisNormalise3DNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Retrieve / realise the normalised data from the supplied list
	bool created;
	Data3D& normalisedData = GenericListHelper<Data3D>::realise(targetList, CharString("%s_%s", name(), cfg->niceName()), prefix, GenericItem::InRestartFileFlag, &created);

	normalisedData.setName(name());
	normalisedData.setObjectTag(CharString("%s//Normalise3D//%s//%s", prefix, cfg->name(), name()));

	// Copy the averaged data from the associated Collect3D node, and normalise it accordingly
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
		return Messenger::error("Saving of 3D data is not yet implemented.\n");
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
bool AnalysisNormalise3DNode::read(LineParser& parser, NodeContextStack& contextStack)
{
	// The current line in the parser must also contain the name of a Collect3D node which we will operate on (it will also become our node name)
	if (parser.nArgs() != 2) return Messenger::error("A Normalise3D node must be given the name of a Collect3D node.\n");
	collectNode_ = contextStack.collect3DNode(parser.argc(1));
	if (!collectNode_) return Messenger::error("A valid Collect3D node name must be given as an argument to Normalise3D.\n");
	setName(parser.argc(1));

	AnalysisSelectBaseNode* selectNode;

	// Read until we encounter the EndNormalise3D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		Normalise3DNodeKeyword nk = normalise3DNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (Normalise3DNodeKeyword::EndNormalise3DKeyword):
				return true;
			case (Normalise3DNodeKeyword::FactorKeyword):
				normalisationFactor_ = parser.argd(1);
				normaliseByFactor_ = true;
				break;
			case (Normalise3DNodeKeyword::LabelValueKeyword):
				valueLabel_ = parser.argc(1);
				break;
			case (Normalise3DNodeKeyword::LabelXKeyword):
				xAxisLabel_ = parser.argc(1);
				break;
			case (Normalise3DNodeKeyword::LabelYKeyword):
				yAxisLabel_ = parser.argc(1);
				break;
			case (Normalise3DNodeKeyword::LabelZKeyword):
				zAxisLabel_ = parser.argc(1);
				break;
			case (Normalise3DNodeKeyword::NSitesKeyword):
				selectNode = contextStack.selectNode(parser.argc(1));
				if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(0), normalise3DNodeKeyword(Normalise3DNodeKeyword::NSitesKeyword));
				sitePopulationNormalisers_.add(selectNode, 1.0);
				break;
			case (Normalise3DNodeKeyword::NumberDensityKeyword):
				selectNode = contextStack.selectNode(parser.argc(1));
				if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(0), normalise3DNodeKeyword(Normalise3DNodeKeyword::NumberDensityKeyword));
				numberDensityNormalisers_.add(selectNode, 1.0);
				break;
			case (Normalise3DNodeKeyword::SaveKeyword):
				saveNormalisedData_ = parser.argb(1);
				break;
			case (Normalise3DNodeKeyword::nNormalise3DNodeKeywords):
				return Messenger::error("Unrecognised Normalise3D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Normalise3D node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisNormalise3DNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
