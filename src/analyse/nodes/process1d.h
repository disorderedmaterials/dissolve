/*
	*** Analysis Node - Process1D
	*** src/analyse/nodes/process1d.h
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

#ifndef DISSOLVE_ANALYSISPROCESS1D_H
#define DISSOLVE_ANALYSISPROCESS1D_H

#include "analyse/nodes/node.h"
#include "math/data1d.h"
#include "base/charstring.h"
#include "templates/reflist.h"

// Forward Declarations
class AnalysisCollect1DNode;
class AnalysisSelectBaseNode;
class Data1D;
class LineParser;
class NodeContextStack;

// Analysis Node - Process1D
class AnalysisProcess1DNode : public AnalysisNode
{
	public:
	// Constructor
	AnalysisProcess1DNode(AnalysisCollect1DNode* target = NULL);
	// Destructor
	~AnalysisProcess1DNode();


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum Process1DNodeKeyword { EndProcess1DKeyword, FactorKeyword, LabelValueKeyword, LabelXKeyword, NSitesKeyword, NumberDensityKeyword, SaveKeyword, SphericalShellVolumeKeyword, nProcess1DNodeKeywords };
	// Convert string to control keyword
	static Process1DNodeKeyword normalise1DNodeKeyword(const char* s);
	// Convert control keyword to string
	static const char* normalise1DNodeKeyword(Process1DNodeKeyword nk);


	/*
	 * Data
	 */
	private:
	// Collect1D node which we are normalising
	AnalysisCollect1DNode* collectNode_;
	// Reference to sites against which we will normalise by population
	RefList<AnalysisSelectBaseNode,double> sitePopulationNormalisers_;
	// Reference to sites against which we will normalise by number density
	RefList<AnalysisSelectBaseNode,double> numberDensityNormalisers_;
	// Whether to normalise by supplied factor
	bool normaliseByFactor_;
	// Normalisation factor to apply (if requested)
	double normalisationFactor_;
	// Whether to normalise by spherical shell volume
	bool normaliseBySphericalShellVolume_;
	// Whether to save data after normalisation
	bool saveData_;
	// Value label
	CharString valueLabel_;
	// Axis labels
	CharString xAxisLabel_;

	public:
	// Add site population normaliser
	void addSitePopulationNormaliser(AnalysisSelectBaseNode* selectNode);
	// Add number density normaliser
	void addNumberDensityNormaliser(AnalysisSelectBaseNode* selectNode);
	// Set whether to normalise by factor
	void setNormaliseByFactor(bool on);
	// Set normalisation factor
	void setNormalisationFactor(double factor);
	// Set whether to normalise by spherical shell volume
	void setNormaliseBySphericalShellVolume(bool on);
	// Set whether to save processed data
	void setSaveData(bool on);
	// Set value label
	void setValueLabel(const char* label);
	// Return value label
	const char* valueLabel() const;
	// Set x axis label
	void setXAxisLabel(const char* label);
	// Return x axis label
	const char* xAxisLabel() const;

	/*
	 * Execute
	 */
	public:
	// Prepare any necessary data, ready for execution
	bool prepare(Configuration* cfg, const char* prefix, GenericList& targetList);
	// Execute node, targetting the supplied Configuration
	AnalysisNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);
	// Finalise any necessary data after execution
	bool finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser, NodeContextStack& contextStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
