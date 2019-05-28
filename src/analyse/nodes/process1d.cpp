/*
	*** Analysis Node - Process1D
	*** src/analyse/nodes/process1d.cpp
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

#include "analyse/nodes/process1d.h"
#include "analyse/nodes/collect1d.h"
#include "analyse/nodes/select.h"
#include "analyse/nodecontextstack.h"
#include "modules/analyse/analyse.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "genericitems/listhelper.h"

// Constructor
AnalysisProcess1DNode::AnalysisProcess1DNode(AnalysisCollect1DNode* target) : AnalysisNode(AnalysisNode::Process1DNode)
{
	collectNode_.addAllowableNodeType(AnalysisNode::Collect1DNode);

	collectNode_ = target;
	processedData_ = NULL;
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

// Node Keywords
const char* Process1DNodeKeywords[] = { "EndProcess1D", "Factor", "LabelValue", "LabelX", "NSites", "NumberDensity", "Save", "SourceData", "SphericalShellVolume" };

// Convert string to node keyword
AnalysisProcess1DNode::Process1DNodeKeyword AnalysisProcess1DNode::process1DNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisProcess1DNode::nProcess1DNodeKeywords; ++nk) if (DissolveSys::sameString(s, Process1DNodeKeywords[nk])) return (AnalysisProcess1DNode::Process1DNodeKeyword) nk;

	return AnalysisProcess1DNode::nProcess1DNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisProcess1DNode::process1DNodeKeyword(AnalysisProcess1DNode::Process1DNodeKeyword nk)
{
	return Process1DNodeKeywords[nk];
}

/*
 * Data
 */

// Return processed data
const Data1D& AnalysisProcess1DNode::processedData() const
{
	if (!processedData_)
	{
		Messenger::error("No processed data pointer set in AnalysisProcess1DNode, so nothing to return.\n");
		static Data1D dummy;
		return dummy;
	}

	return (*processedData_);
}

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
	processedData_ = &data;

	data.setName(name());
	data.setObjectTag(CharString("%s//Process1D//%s//%s", prefix, cfg->name(), name()));

	// Get the node pointer
	AnalysisCollect1DNode* node = (AnalysisCollect1DNode*) collectNode_.node();
	if (!node) return Messenger::error("No Collect1D node available in AnalysisProcess1DNode::finalise().\n");

	// Copy the averaged data from the associated Collect1D node, and normalise it accordingly
	data = node->accumulatedData();

	// Normalisation by number of sites?
	RefListIterator<AnalysisSelectNode,double> siteNormaliserIterator(sitePopulationNormalisers_);
	while (AnalysisSelectNode* selectNode = siteNormaliserIterator.iterate()) data /= selectNode->nAverageSites();

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
bool AnalysisProcess1DNode::read(LineParser& parser, const CoreData& coreData, NodeContextStack& contextStack)
{
	// The current line in the parser may contain a node name for us
	if (parser.nArgs() == 2) setName(parser.argc(1));

	// Add ourselves to the context stack
	if (!contextStack.add(this)) return Messenger::error("Error adding Process1D node '%s' to context stack.\n", name());

	AnalysisSelectNode* selectNode;

	// Read until we encounter the EndProcess1D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		Process1DNodeKeyword nk = process1DNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (AnalysisProcess1DNode::EndProcess1DKeyword):
				return true;
			case (AnalysisProcess1DNode::FactorKeyword):
				normalisationFactor_ = parser.argd(1);
				normaliseByFactor_ = true;
				break;
			case (AnalysisProcess1DNode::LabelValueKeyword):
				valueLabel_ = parser.argc(1);
				break;
			case (AnalysisProcess1DNode::LabelXKeyword):
				xAxisLabel_ = parser.argc(1);
				break;
			case (AnalysisProcess1DNode::NSitesKeyword):
				// Need a valid collectNode_ so we can retrieve the context stack it's local to
				if (collectNode_.isNull()) return Messenger::error("Can't set site-dependent normalisers without first setting the collect node target.\n");
				if (!collectNode_.node()->parent()) return Messenger::error("Can't set site-dependent normalisers since the specified collect node has no analyser parent.\n");

				selectNode = (AnalysisSelectNode*) collectNode_.node()->parent()->contextStack().node(parser.argc(1), AnalysisNode::SelectNode);
				if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(0), process1DNodeKeyword(AnalysisProcess1DNode::NSitesKeyword));
				sitePopulationNormalisers_.add(selectNode, 1.0);
				break;
			case (AnalysisProcess1DNode::NumberDensityKeyword):
				// Need a valid collectNode_ so we can retrieve the context stack it's local to
				if (collectNode_.isNull()) return Messenger::error("Can't set site-dependent normalisers without first setting the collect node target.\n");
				if (!collectNode_.node()->parent()) return Messenger::error("Can't set site-dependent normalisers since the specified collect node has no analyser parent.\n");

				selectNode = (AnalysisSelectNode*) collectNode_.node()->parent()->contextStack().node(parser.argc(1), AnalysisNode::SelectNode);
				if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(0), process1DNodeKeyword(AnalysisProcess1DNode::NumberDensityKeyword));
				numberDensityNormalisers_.add(selectNode, 1.0);
				break;
			case (AnalysisProcess1DNode::SaveKeyword):
				saveData_ = parser.argb(1);
				break;
			case (AnalysisProcess1DNode::SourceDataKeyword):
				if (!collectNode_.read(parser, 1, coreData, contextStack)) return Messenger::error("Couldn't set source data for node.\n");
				break;
			case (AnalysisProcess1DNode::SphericalShellVolumeKeyword):
				normaliseBySphericalShellVolume_ = parser.argb(1);
				break;
			case (AnalysisProcess1DNode::nProcess1DNodeKeywords):
				return Messenger::error("Unrecognised Process1D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Process1D node keyword '%s'.\n", parser.argc(0));
		}
	}

	// Check that a valid collectNode_ has been set
	if (collectNode_.isNull()) return Messenger::error("A valid Collect1D node must be set in the Process1D node '%s' using the '%s' keyword.\n", name(), process1DNodeKeyword(AnalysisProcess1DNode::SourceDataKeyword));

	return true;
}

// Write structure to specified LineParser
bool AnalysisProcess1DNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
