/*
	*** NETA Definition
	*** src/neta/neta.h
	Copyright T. Youngs 2019-2020

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

#ifndef DISSOLVE_NETA_H
#define DISSOLVE_NETA_H

#include "neta/connection.h"
#include "neta/logic.h"
#include "neta/root.h"
#include "base/charstring.h"
#include "base/enumoptions.h"
#include "templates/list.h"

// Forward Declarations
class Forcefield;
class SpeciesAtom;

// NETA Definition
class NETADefinition
{
	public:
	// Constructor
	NETADefinition(const char* netaDefinition = NULL, const Forcefield* associatedFF = NULL);
	// Destructor
	~NETADefinition();


	/*
	 * Data
	 */
	private:
	// Root node of the definition
	NETARootNode rootNode_;
	// Original definition string
	CharString definitionString_;

	public:
	// Clear all definition data
	void clear();
	// Return root node pointer
	NETARootNode* rootNode();
	// Set NETADefinition from supplied string
	bool set(const char* netaDefinition, const Forcefield* associatedFF = NULL);
	// Set generating string
	void setDefinitionString(const char* definition);
	// Return original generating string
	const char* definitionString() const;


	/*
	 * Matching
	 */
	private:
	// Current SpeciesAtom being typed
	SpeciesAtom* targetAtom_;

	public:
	// Check supplied atom to see if it matches this NETA description
	int score(const SpeciesAtom* i) const;
};

#endif
