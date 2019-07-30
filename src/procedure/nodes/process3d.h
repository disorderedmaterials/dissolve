/*
	*** Procedure Node - Process3D
	*** src/procedure/nodes/process3d.h
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

#ifndef DISSOLVE_PROCEDURENODE_PROCESS3D_H
#define DISSOLVE_PROCEDURENODE_PROCESS3D_H

#include "procedure/nodes/node.h"
#include "procedure/nodes/nodereference.h"
#include "math/data3d.h"
#include "base/charstring.h"
#include "templates/reflist.h"

// Forward Declarations
class Collect3DProcedureNode;
class SelectProcedureNode;
class Data3D;
class LineParser;
class NodeScopeStack;

// Procedure Node - Process3D
class Process3DProcedureNode : public ProcedureNode
{
	public:
	// Constructor
	Process3DProcedureNode(Collect3DProcedureNode* target = NULL);
	// Destructor
	~Process3DProcedureNode();


	/*
	 * Identity
	 */
	public:
	// Return whether specified context is relevant for this node type
	bool isContextRelevant(ProcedureNode::NodeContext context);


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum Process3DNodeKeyword { EndProcess3DKeyword, FactorKeyword, LabelValueKeyword, LabelXKeyword, LabelYKeyword, LabelZKeyword, NSitesKeyword, NumberDensityKeyword, SaveKeyword, SourceDataKeyword, nProcess3DNodeKeywords };
	// Return enum option info for Process3DNodeKeyword
	static EnumOptions<Process3DNodeKeyword> process3DNodeKeywords();


	/*
	 * Data
	 */
	private:
	// Collect3D node that we are processing
	ProcedureNodeReference collectNode_;
	// Pointer to processed data (stored in processing data list)
	Data3D* processedData_;
	// Reference to sites against which we will normalise by population
	RefList<SelectProcedureNode> sitePopulationNormalisers_;
	// Reference to sites against which we will normalise by number density
	RefList<SelectProcedureNode> numberDensityNormalisers_;
	// Whether to normalise by supplied factor
	bool normaliseByFactor_;
	// Normalisation factor to apply (if requested)
	double normalisationFactor_;
	// Whether to save data after normalisation
	bool saveData_;
	// Value label
	CharString valueLabel_;
	// Axis labels
	CharString xAxisLabel_, yAxisLabel_, zAxisLabel_;

	public:
	// Return processed data
	const Data3D& processedData() const;
	// Add site population normaliser
	void addSitePopulationNormaliser(SelectProcedureNode* selectNode);
	// Add number density normaliser
	void addNumberDensityNormaliser(SelectProcedureNode* selectNode);
	// Set whether to normalise by factor
	void setNormaliseByFactor(bool on);
	// Set normalisation factor
	void setNormalisationFactor(double factor);
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
	// Set y axis label
	void setYAxisLabel(const char* label);
	// Return y axis label
	const char* yAxisLabel() const;
	// Set z axis label
	void setZAxisLabel(const char* label);
	// Return z axis label
	const char* zAxisLabel() const;


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
	bool read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
