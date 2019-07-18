/*
	*** Procedure Node - Calculate
	*** src/procedure/nodes/calculate.h
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

#ifndef DISSOLVE_PROCEDURENODE_CALCULATE_H
#define DISSOLVE_PROCEDURENODE_CALCULATE_H

#include "procedure/nodes/node.h"
#include "templates/array.h"

// Forward Declarations
class SelectProcedureNode;

// Select Node
class CalculateProcedureNode : public ProcedureNode
{
	/*
	 * Observables
	 */
	public:
	// Observables
	enum Observable
	{
		AngleObservable,		/* 'Angle' - Angle formed between three sites */
		DistanceObservable,		/* 'Distance' - Distance between two sites */
		VectorObservable,		/* 'Vector' - Minimum image vector between two sites */
		nObservables			/* Number of observables in list */
	};
	// Convert string to Observable
	static Observable observable(const char* s);
	// Convert Observable to string
	static const char* observable(Observable obs);
	// Number of sites required to calculate Observable
	static int observableNSites(Observable obs);
	// Dimensionality of data for Observable
	static int observableDimensionality(Observable obs);
	// Constructor
	CalculateProcedureNode(CalculateProcedureNode::Observable observable = nObservables, SelectProcedureNode* site0 = NULL, SelectProcedureNode* site1 = NULL, SelectProcedureNode* site2 = NULL, SelectProcedureNode* site3 = NULL);
	// Destructor
	~CalculateProcedureNode();


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum CalculateNodeKeyword { AngleKeyword, DistanceKeyword, EndCalculateKeyword, VectorKeyword, nCalculateNodeKeywords };
	// Convert string to control keyword
	static CalculateNodeKeyword calculateNodeKeyword(const char* s);
	// Convert control keyword to string
	static const char* calculateNodeKeyword(CalculateNodeKeyword nk);


	/*
	 * Observable Target
	 */
	private:
	// Target observable to calculate
	Observable observable_;
	// Nodes (sites) to use for observable calculation
	SelectProcedureNode* sites_[4];
	// Last calculate value(s) of observable (as Vec3)
	Vec3<double> value_;

	public:
	// Return last calculated value of observable
	double value(int id) const;
	// Return last calculated value as vector
	Vec3<double> values() const;


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
	bool read(LineParser& parser, const CoreData& coreData, NodeContextStack& contextStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
