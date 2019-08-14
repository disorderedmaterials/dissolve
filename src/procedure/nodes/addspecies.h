/*
	*** Procedure Node - AddSpecies
	*** src/procedure/nodes/addspecies.h
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

#ifndef DISSOLVE_PROCEDURENODE_ADDSPECIES_H
#define DISSOLVE_PROCEDURENODE_ADDSPECIES_H

#include "procedure/nodes/node.h"
#include "procedure/nodevalue.h"
#include "base/units.h"

// Forward Declarations
class Species;

// AddSpecies Node
class AddSpeciesProcedureNode : public ProcedureNode
{
	public:
	// Constructor
	AddSpeciesProcedureNode(Species* sp = NULL, int population = 0, double atomicDensity = -1.0);
	// Destructor
	~AddSpeciesProcedureNode();


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
	enum AddSpeciesNodeKeyword { DensityKeyword, EndAddSpeciesKeyword, NoRotationKeyword, PopulationKeyword, PositionKeyword, SpeciesKeyword, nAddSpeciesNodeKeywords };
	// Return enum option info for AddSpeciesNodeKeyword
	static EnumOptions<AddSpeciesNodeKeyword> addSpeciesNodeKeywords();


	/*
	 * Node Data
	 */
	public:
	// Positioning Type
	enum PositioningType
	{
		CentralPositioning,			/* Position the Species at the centre of the Box */
		CurrentPositioning,			/* Use current Species coordinates */
		RandomPositioning,			/* Set position randomly */
		nPositioningTypes
	};
	// Return enum option info for PositioningType
	static EnumOptions<PositioningType> positioningTypes();

	private:
	// Target Species to add
	Species* species_;
	// Integer population of the Species to add
	NodeValue population_;
	// Density at which to add the specified Species
	NodeValue density_;
	// Units of the supplied density
	Units::DensityUnits densityUnits_;
	// Whether to rotate molecules randomly on addition
	bool rotate_;
	// Positioning of individual molecules
	PositioningType positioning_;

	public:
	// Set whether to rotate molecules randomly on addition
	void setRotate(bool rotate);
	// Set positioning of individual molecules
	void setPositioning(PositioningType posType);


	/*
	 * Execute
	 */
	public:
	// Prepare any necessary data, ready for execution
	bool prepare(Configuration* cfg, const char* prefix, GenericList& targetList);
	// Execute node, targetting the supplied Configuration
	ProcedureNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);


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
