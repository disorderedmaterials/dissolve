/*
	*** Analysis Node - Normalise1D
	*** src/analyse/nodes/normalise1d.cpp
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

#include "analyse/nodes/normalise1d.h"
#include "analyse/nodes/collect1d.h"
#include "analyse/nodes/select.h"
#include "analyse/nodecontextstack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Constructor
AnalysisNormalise1DNode::AnalysisNormalise1DNode(AnalysisCollect1DNode* target) : AnalysisNode(AnalysisNode::Normalise1DNode)
{
	collectNode_ = target;
	saveNormalisedData_ = false;
	normalisationFactor_ = 0.0;
	normaliseByFactor_ = false;
	normaliseBySphericalShellVolume_ = false;
}

// Destructor
AnalysisNormalise1DNode::~AnalysisNormalise1DNode()
{
}

/*
 * Node Keywords
 */

// Node Keywords (note ordering for efficiency)
const char* Normalise1DNodeKeywords[] = { "EndNormalise1D", "Factor", "LabelValue", "LabelX", "NSites", "NumberDensity", "Save", "SphericalShellVolume" };

// Convert string to node keyword
AnalysisNormalise1DNode::Normalise1DNodeKeyword AnalysisNormalise1DNode::normalise1DNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisNormalise1DNode::nNormalise1DNodeKeywords; ++nk) if (DissolveSys::sameString(s, Normalise1DNodeKeywords[nk])) return (AnalysisNormalise1DNode::Normalise1DNodeKeyword) nk;

	return AnalysisNormalise1DNode::nNormalise1DNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisNormalise1DNode::normalise1DNodeKeyword(AnalysisNormalise1DNode::Normalise1DNodeKeyword nk)
{
	return Normalise1DNodeKeywords[nk];
}

/*
 * Data
 */

// Add site population normaliser
void AnalysisNormalise1DNode::addSitePopulationNormaliser(AnalysisSelectBaseNode* selectNode)
{
	sitePopulationNormalisers_.add(selectNode, 1.0);
}

// Add number density normaliser
void AnalysisNormalise1DNode::addNumberDensityNormaliser(AnalysisSelectBaseNode* selectNode)
{
	numberDensityNormalisers_.add(selectNode);
}

// Set whether to normalise by factor
void AnalysisNormalise1DNode::setNormaliseByFactor(bool on)
{
	normaliseByFactor_ = on;
}

// Set normalisation factor
void AnalysisNormalise1DNode::setNormalisationFactor(double factor)
{
	normalisationFactor_ = factor;
}

// Set whether to normalise by spherical shell volume
void AnalysisNormalise1DNode::setNormaliseBySphericalShellVolume(bool on)
{
	normaliseBySphericalShellVolume_ = on;
}

// Set whether to save normalised data
void AnalysisNormalise1DNode::setSaveNormalisedData(bool on)
{
	saveNormalisedData_ = on;
}

// Set value label
void AnalysisNormalise1DNode::setValueLabel(const char* label)
{
	valueLabel_ = label;
}

// Return value label
const char* AnalysisNormalise1DNode::valueLabel() const
{
	return valueLabel_.get();
}

// Set x axis label
void AnalysisNormalise1DNode::setXAxisLabel(const char* label)
{
	xAxisLabel_ = label;
}

// Return x axis label
const char* AnalysisNormalise1DNode::xAxisLabel() const
{
	return xAxisLabel_.get();
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AnalysisNormalise1DNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return true;
}

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisNormalise1DNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return AnalysisNode::Success;
}

// Finalise any necessary data after execution
bool AnalysisNormalise1DNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Retrieve / realise the normalised data from the supplied list
	bool created;
	Data1D& normalisedData = GenericListHelper<Data1D>::realise(targetList, CharString("%s_%s", name(), cfg->niceName()), prefix, GenericItem::InRestartFileFlag, &created);

	normalisedData.setName(name());
	normalisedData.setObjectTag(CharString("%s//Normalise1D//%s//%s", prefix, cfg->name(), name()));

	// Copy the averaged data from the associated Collect1D node, and normalise it accordingly
	normalisedData = collectNode_->accumulatedData();

	// Normalisation by number of sites?
	RefListIterator<AnalysisSelectBaseNode,double> siteNormaliserIterator(sitePopulationNormalisers_);
	while (AnalysisSelectBaseNode* selectNode = siteNormaliserIterator.iterate()) normalisedData /= selectNode->nAverageSites();

	// Normalisation by spherical shell?
	if (normaliseBySphericalShellVolume_)
	{
		double halfBinWidth = collectNode_->binWidth() * 0.5;
		double r1Cubed = pow(normalisedData.xAxis(0)-halfBinWidth,3), r2Cubed;
		for (int n = 0; n < normalisedData.nDataPoints(); ++n)
		{
			r2Cubed = pow(normalisedData.xAxis(n)+halfBinWidth,3);
			normalisedData.value(n) /= (4.0/3.0) * PI * (r2Cubed - r1Cubed);
			normalisedData.error(n) /= (4.0/3.0) * PI * (r2Cubed - r1Cubed);
			r1Cubed = r2Cubed;
		}
	}

	// Normalisation by number density of sites?
	RefListIterator<AnalysisSelectBaseNode,double> numberDensityIterator(numberDensityNormalisers_);
	while (AnalysisSelectBaseNode* selectNode = numberDensityIterator.iterate()) normalisedData /= (selectNode->nAverageSites() / cfg->box()->volume());

	// Normalisation by factor?
	if (normaliseByFactor_) normalisedData /= normalisationFactor_;

	// Save data?
	if (saveNormalisedData_ && procPool.isMaster())
	{
		if (normalisedData.save(CharString("%s_%s.txt", name(), cfg->name()))) procPool.decideTrue();
		else return procPool.decideFalse();
	}
	else if (!procPool.decision()) return false;

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisNormalise1DNode::read(LineParser& parser, NodeContextStack& contextStack)
{
	// The current line in the parser must also contain the name of a Collect1D node which we will operate on (it will also become our node name)
	if (parser.nArgs() != 2) return Messenger::error("A Normalise1D node must be given the name of a Collect1D node.\n");
	collectNode_ = contextStack.collect1DNode(parser.argc(1));
	if (!collectNode_) return Messenger::error("A valid Collect1D node name must be given as an argument to Normalise1D.\n");
	setName(parser.argc(1));

	AnalysisSelectBaseNode* selectNode;

	// Read until we encounter the EndNormalise1D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		Normalise1DNodeKeyword nk = normalise1DNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (Normalise1DNodeKeyword::EndNormalise1DKeyword):
				return true;
			case (Normalise1DNodeKeyword::FactorKeyword):
				normalisationFactor_ = parser.argd(1);
				normaliseByFactor_ = true;
				break;
			case (Normalise1DNodeKeyword::LabelValueKeyword):
				valueLabel_ = parser.argc(1);
				break;
			case (Normalise1DNodeKeyword::LabelXKeyword):
				xAxisLabel_ = parser.argc(1);
				break;
			case (Normalise1DNodeKeyword::NSitesKeyword):
				selectNode = contextStack.selectNode(parser.argc(1));
				if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(0), normalise1DNodeKeyword(Normalise1DNodeKeyword::NSitesKeyword));
				sitePopulationNormalisers_.add(selectNode, 1.0);
				break;
			case (Normalise1DNodeKeyword::NumberDensityKeyword):
				selectNode = contextStack.selectNode(parser.argc(1));
				if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(0), normalise1DNodeKeyword(Normalise1DNodeKeyword::NumberDensityKeyword));
				numberDensityNormalisers_.add(selectNode, 1.0);
				break;
			case (Normalise1DNodeKeyword::SaveKeyword):
				saveNormalisedData_ = parser.argb(1);
				break;
			case (Normalise1DNodeKeyword::SphericalShellVolumeKeyword):
				normaliseBySphericalShellVolume_ = parser.argb(1);
				break;
			case (Normalise1DNodeKeyword::nNormalise1DNodeKeywords):
				return Messenger::error("Unrecognised Normalise1D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Normalise1D node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisNormalise1DNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
