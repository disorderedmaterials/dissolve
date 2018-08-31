/*
	*** Analysis Node - Normalise1D
	*** src/analyse/nodes/normalise1d.h
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

#ifndef DISSOLVE_ANALYSISNORMALISE1D_H
#define DISSOLVE_ANALYSISNORMALISE1D_H

#include "analyse/nodes/node.h"
#include "math/data1d.h"
#include "base/charstring.h"
#include "templates/reflist.h"

// Forward Declarations
class AnalysisCollect1DNode;
class Data1D;
class LineParser;
class NodeContextStack;

// Analysis Node - Normalise1D
class AnalysisNormalise1DNode : public AnalysisNode
{
	public:
	// Constructor
	AnalysisNormalise1DNode();
	// Destructor
	~AnalysisNormalise1DNode();


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum Normalise1DNodeKeyword { EndNormalise1DKeyword, NSitesKeyword, nNormalise1DNodeKeywords };
	// Convert string to control keyword
	static Normalise1DNodeKeyword normalise1DNodeKeyword(const char* s);
	// Convert control keyword to string
	static const char* normalise1DNodeKeyword(Normalise1DNodeKeyword nk);


	/*
	 * Data
	 */
	private:
	// Collect1D node which we are normalising
	AnalysisCollect1DNode* collectNode_;
	// Normalised data, calculated from accumulated histogram data in associated Collect1D node
	Data1D normalisedData_;


	/*
	 * Execute
	 */
	public:
	// Prepare any necessary data, ready for execution
	bool prepare(Configuration* cfg, const char* dataPrefix, GenericList& targetList);
	// Execute node, targetting the supplied Configuration
	AnalysisNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* dataPrefix, GenericList& targetList);
	// Finalise any necessary data after execution
	bool finalise(Configuration* cfg, const char* dataPrefix, GenericList& targetList);


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
