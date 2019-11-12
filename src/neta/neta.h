/*
	*** NETA Definition
	*** src/neta/neta.h
	Copyright T. Youngs 2019

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

#include "neta/node.h"
#include "neta/connection.h"
#include "neta/logic.h"
#include "base/charstring.h"
#include "base/enumoptions.h"
#include "templates/list.h"

// Forward Declarations
class Forcefield;
class NETARootNode;
class SpeciesAtom;

// NETA Definition
class NETADefinition
{
	public:
	// Constructor
	NETADefinition(const char* netaDefinition = NULL, const Forcefield* associatedFF = NULL);
	// Destructor
	~NETADefinition();
// 	// NETA Keywords
// 	enum NETAKeyword { AliphaticKeyword, AromaticKeyword, NoRingKeyword, NonAromaticKeyword, NotPrevKeyword, NotSelfKeyword, PlanarKeyword, nNETAKeywords };
// 	// Return enum options for NETAKeyword
// 	static EnumOptions<NETADefinition::NETAKeyword> netaKeywords();
// 	// NETA Expanders
// 	enum NETAExpander { BoundExpanded, ChainExpander, DoublyBoundExpander, GeometryExpander, PathExpander, RingExpander, nNETAExpanders };
// 	// Return enum options for NETAExpander
// 	static EnumOptions<NETADefinition::NETAExpander> netaExpanders();
	// NETA values
// 	enum NETAValue { BondValue, NBondsValue, NHydrogensValue, OxidationStateValue, RepeatValue, SizeValue, nNETAValues };
	// Return enum options for NETAValue
// 	static EnumOptions<NETADefinition::NETAValue> netaValues();
// 	// Friend Class
// 	friend class NETAParser;


	/*
	 * Data
	 */
	private:
	// Root node of the definition
	NETANode rootNode_;
	// Original definition string
	CharString definitionString_;

	public:
	// Clear all definition data
	void clear();
	// Return root node pointer
	NETANode* rootNode();
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
