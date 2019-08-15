/*
	*** Procedure Node - Process1D
	*** src/procedure/nodes/process1d.h
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

#ifndef DISSOLVE_PROCEDURENODE_PROCESS1D_H
#define DISSOLVE_PROCEDURENODE_PROCESS1D_H

#include "procedure/nodes/node.h"
#include "procedure/nodes/nodereference.h"
#include "math/data1d.h"
#include "base/charstring.h"
#include "templates/reflist.h"

// Forward Declarations
class Collect1DProcedureNode;
class SelectProcedureNode;
class Data1D;
class LineParser;
class NodeScopeStack;

// Procedure Node - Process1D
class Process1DProcedureNode : public ProcedureNode
{
	public:
	// Constructor
	Process1DProcedureNode(Collect1DProcedureNode* target = NULL);
	// Destructor
	~Process1DProcedureNode();


	/*
	 * Identity
	 */
	public:
	// Return whether specified context is relevant for this node type
	bool isContextRelevant(ProcedureNode::NodeContext context);


	/*
	 * Node Keywords
	 */
	private:
	// Set up keywords for node
	void setUpKeywords();

	public:
	// Node Keywords
	enum Process1DNodeKeyword { EndProcess1DKeyword, FactorKeyword, LabelValueKeyword, LabelXKeyword, NormaliseToOneKeyword, NSitesKeyword, NumberDensityKeyword, SaveKeyword, SourceDataKeyword, SphericalShellVolumeKeyword, nProcess1DNodeKeywords };
	// Return enum option info for Process1DNodeKeyword
	static EnumOptions<Process1DNodeKeyword> process1DNodeKeywords();


	/*
	 * Data
	 */
	private:
	// Collect1D node that we are processing
	ProcedureNodeReference collectNode_;
	// Pointer to processed data (stored in processing data list)
	Data1D* processedData_;
	// Reference to sites against which we will normalise by population
	RefList<SelectProcedureNode> sitePopulationNormalisers_;
	// Reference to sites against which we will normalise by number density
	RefList<SelectProcedureNode> numberDensityNormalisers_;
	// Whether to normalise by supplied factor
	bool normaliseByFactor_;
	// Normalisation factor to apply (if requested)
	double normalisationFactor_;
	// Whether to normalise by spherical shell volume
	bool normaliseBySphericalShellVolume_;
	// Whether to normalise the data to one
	bool normaliseToOne_;
	// Whether to save data after normalisation
	bool saveData_;
	// Value label
	CharString valueLabel_;
	// Axis labels
	CharString xAxisLabel_;

	public:
	// Return processed data
	const Data1D& processedData() const;
	// Add site population normaliser
	void addSitePopulationNormaliser(SelectProcedureNode* selectNode);
	// Add number density normaliser
	void addNumberDensityNormaliser(SelectProcedureNode* selectNode);
	// Set whether to normalise by factor
	void setNormaliseByFactor(bool on);
	// Set normalisation factor
	void setNormalisationFactor(double factor);
	// Set whether to normalise by spherical shell volume
	void setNormaliseBySphericalShellVolume(bool on);
	// Set whether to normalise to one
	void setNormaliseToOne(bool on);
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
	ProcedureNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);
	// Finalise any necessary data after execution
	bool finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser, const CoreData& coreData);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
