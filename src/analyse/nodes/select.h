/*
	*** Analysis Node - Select
	*** src/analyse/nodes/select.h
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

#ifndef DISSOLVE_ANALYSISSELECT_H
#define DISSOLVE_ANALYSISSELECT_H

#include "analyse/nodes/node.h"

// Forward Declarations
/* none */

// Select Node
class AnalysisSelectNode : public AnalysisNode
{
	public:
	// Constructor
	AnalysisSelectNode();
	// Destructor
	~AnalysisSelectNode();


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum SelectNodeKeyword { nSelectNodeKeywords };
	// Convert string to control keyword
	static SelectNodeKeyword selectNodeKeyword(const char* s);
	// Convert control keyword to string
	static const char* selectNodeKeyword(SelectNodeKeyword nk);


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
