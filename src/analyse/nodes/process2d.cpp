/*
	*** Analysis Node - Process2D
	*** src/analyse/nodes/process2d.cpp
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

#include "analyse/nodes/process2d.h"
#include "analyse/nodes/collect2d.h"
#include "analyse/nodes/select.h"
#include "analyse/nodecontextstack.h"
#include "modules/export/export.h"
#include "math/integrator.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "genericitems/listhelper.h"

// Constructor
AnalysisProcess2DNode::AnalysisProcess2DNode(AnalysisCollect2DNode* target) : AnalysisNode(AnalysisNode::Process2DNode)
{
	collectNode_.addAllowableNodeType(AnalysisNode::Collect2DNode);

	collectNode_ = target;
	processedData_ = NULL;
	saveData_ = false;
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

// Node Keywords
const char* Process2DNodeKeywords[] = { "EndProcess2D", "Factor", "LabelValue", "LabelX", "LabelY", "NormaliseToOne", "NSites", "NumberDensity", "Save", "SourceData" };

// Convert string to node keyword
AnalysisProcess2DNode::Process2DNodeKeyword AnalysisProcess2DNode::process2DNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisProcess2DNode::nProcess2DNodeKeywords; ++nk) if (DissolveSys::sameString(s, Process2DNodeKeywords[nk])) return (AnalysisProcess2DNode::Process2DNodeKeyword) nk;

	return AnalysisProcess2DNode::nProcess2DNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisProcess2DNode::process2DNodeKeyword(AnalysisProcess2DNode::Process2DNodeKeyword nk)
{
	return Process2DNodeKeywords[nk];
}

/*
 * Data
 */

// Return processed data
const Data2D& AnalysisProcess2DNode::processedData() const
{
	if (!processedData_)
	{
		Messenger::error("No processed data pointer set in AnalysisProcess2DNode, so nothing to return.\n");
		static Data2D dummy;
		return dummy;
	}

	return (*processedData_);
}

// Add site population normaliser
void AnalysisProcess2DNode::addSitePopulationNormaliser(AnalysisSelectNode* selectNode)
{
	sitePopulationNormalisers_.add(selectNode, 1.0);
}

// Add number density normaliser
void AnalysisProcess2DNode::addNumberDensityNormaliser(AnalysisSelectNode* selectNode)
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

// Set whether to save processed data
void AnalysisProcess2DNode::setSaveData(bool on)
{
	saveData_ = on;
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

// Set y axis label
void AnalysisProcess2DNode::setYAxisLabel(const char* label)
{
	yAxisLabel_ = label;
}

// Return y axis label
const char* AnalysisProcess2DNode::yAxisLabel() const
{
	return yAxisLabel_.get();
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
	// Retrieve / realise the normalised data from the supplied list
	bool created;
	Data2D& data = GenericListHelper<Data2D>::realise(targetList, CharString("%s_%s", name(), cfg->niceName()), prefix, GenericItem::InRestartFileFlag, &created);

	data.setName(name());
	data.setObjectTag(CharString("%s//Process2D//%s//%s", prefix, cfg->name(), name()));
	processedData_ = &data;

	// Get the node pointer
	AnalysisCollect2DNode* node = dynamic_cast<AnalysisCollect2DNode*>(collectNode_.node());
	if (!node) return Messenger::error("No Collect2D node available in AnalysisProcess2DNode::finalise().\n");

	// Copy the averaged data from the associated Collect2D node, and normalise it accordingly
	data = node->accumulatedData();

	// Normalisation by number of sites?
	RefListIterator<AnalysisSelectNode,double> siteNormaliserIterator(sitePopulationNormalisers_);
	while (AnalysisSelectNode* selectNode = siteNormaliserIterator.iterate()) data /= selectNode->nAverageSites();

	// Normalisation by number density of sites?
	RefListIterator<AnalysisSelectNode,double> numberDensityIterator(numberDensityNormalisers_);
	while (AnalysisSelectNode* selectNode = numberDensityIterator.iterate()) data /= (selectNode->nAverageSites() / cfg->box()->volume());

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
		if (ExportModule::writeData2D(Data2DExportFileFormat::BlockData, CharString("%s_%s.txt", name(), cfg->name()), data)) procPool.decideTrue();
		else return procPool.decideFalse();
	}
	else if (!procPool.decision()) return false;

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisProcess2DNode::read(LineParser& parser, const CoreData& coreData, NodeContextStack& contextStack)
{
	// The current line in the parser may contain a node name for us
	if (parser.nArgs() == 2) setName(parser.argc(1));

	// Add ourselves to the context stack
	if (!contextStack.add(this)) return Messenger::error("Error adding Process2D node '%s' to context stack.\n", name());

	AnalysisSelectNode* selectNode;

	// Read until we encounter the EndProcess2D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		Process2DNodeKeyword nk = process2DNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (AnalysisProcess2DNode::EndProcess2DKeyword):
				return true;
			case (AnalysisProcess2DNode::FactorKeyword):
				normalisationFactor_ = parser.argd(1);
				normaliseByFactor_ = true;
				break;
			case (AnalysisProcess2DNode::LabelValueKeyword):
				valueLabel_ = parser.argc(1);
				break;
			case (AnalysisProcess2DNode::LabelXKeyword):
				xAxisLabel_ = parser.argc(1);
				break;
			case (AnalysisProcess2DNode::LabelYKeyword):
				yAxisLabel_ = parser.argc(1);
				break;
			case (AnalysisProcess2DNode::NormaliseToOneKeyword):
				normaliseToOne_ = parser.argb(1);
				break;
			case (AnalysisProcess2DNode::NSitesKeyword):
				// Need a valid collectNode_ so we can retrieve the context stack it's local to
				if (collectNode_.isNull()) return Messenger::error("Can't set site-dependent normalisers without first setting the collect node target.\n");
				if (!collectNode_.node()->parent()) return Messenger::error("Can't set site-dependent normalisers since the specified collect node has no analyser parent.\n");

				selectNode = dynamic_cast<AnalysisSelectNode*>(contextStack.node(parser.argc(1), AnalysisNode::SelectNode));
				if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(1), process2DNodeKeyword(AnalysisProcess2DNode::NSitesKeyword));
				sitePopulationNormalisers_.add(selectNode, 1.0);
				break;
			case (AnalysisProcess2DNode::NumberDensityKeyword):
				// Need a valid collectNode_ so we can retrieve the context stack it's local to
				if (collectNode_.isNull()) return Messenger::error("Can't set site-dependent normalisers without first setting the collect node target.\n");
				if (!collectNode_.node()->parent()) return Messenger::error("Can't set site-dependent normalisers since the specified collect node has no analyser parent.\n");

				selectNode = dynamic_cast<AnalysisSelectNode*>(contextStack.node(parser.argc(1), AnalysisNode::SelectNode));
				if (!selectNode) return Messenger::error("Unrecognised site name '%s' given to '%s' keyword.\n", parser.argc(1), process2DNodeKeyword(AnalysisProcess2DNode::NumberDensityKeyword));
				numberDensityNormalisers_.add(selectNode, 1.0);
				break;
			case (AnalysisProcess2DNode::SaveKeyword):
				saveData_ = parser.argb(1);
				break;
			case (AnalysisProcess2DNode::SourceDataKeyword):
				if (!collectNode_.read(parser, 1, coreData, contextStack)) return Messenger::error("Couldn't set source data for node.\n");
				break;
			case (AnalysisProcess2DNode::nProcess2DNodeKeywords):
				return Messenger::error("Unrecognised Process2D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Process2D node keyword '%s'.\n", parser.argc(0));
		}
	}

	// Check that a valid collectNode_ has been set
	if (collectNode_.isNull()) return Messenger::error("A valid Collect2D node must be set in the Process2D node '%s' using the '%s' keyword.\n", name(), process2DNodeKeyword(AnalysisProcess2DNode::SourceDataKeyword));

	return true;
}

// Write structure to specified LineParser
bool AnalysisProcess2DNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
