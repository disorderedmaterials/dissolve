/*
	*** Empirical Formula Generation
	*** src/classes/empiricalformula.cpp
	Copyright T. Youngs 2012-2020

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

#include "classes/empiricalformula.h"
#include "classes/species.h"
#include "data/elements.h"

// Constructor
EmpiricalFormula::EmpiricalFormula()
{
	// Initialise element counts array
	elementCounts_.initialise(Elements::nElements());
}

// Destructor
EmpiricalFormula::~EmpiricalFormula()
{
}

/*
 * Construction
 */

// Clear counts array
void EmpiricalFormula::reset()
{
	for (int n=0; n<Elements::nElements(); ++n) elementCounts_[n] = 0;
}

// Add to empirical formula
void EmpiricalFormula::add(Element* element, int count)
{
#ifdef CHECKS
	if (element == NULL)
	{
		Messenger::error("EmpiricalFormula::add() - Element is NULL.\n");
		return;
	}
#endif
	elementCounts_[element->Z()] += count;
}

// Return current empirical formula
const char* EmpiricalFormula::formula()
{
	formula_.clear();

	// Loop over elements in descending order
	for (int n=Elements::nElements()-1; n>=0; --n)
	{
		if (elementCounts_[n] == 0) continue;
		else if (elementCounts_[n] > 1) formula_.strcatf("%s%i", Elements::symbol(n), elementCounts_[n]);
		else formula_.strcatf("%s", Elements::symbol(n));
	}

	return formula_.get();
}

// Return rich text of current empirical formula
const char* EmpiricalFormula::richTextFormula()
{
	formula_.clear();

	// Loop over elements in descending order
	for (int n=Elements::nElements()-1; n>=0; --n)
	{
		if (elementCounts_[n] == 0) continue;
		else if (elementCounts_[n] > 1) formula_.strcatf("%s<sub>%i</sub>", Elements::symbol(n), elementCounts_[n]);
		else formula_.strcatf("%s", Elements::symbol(n));
	}

	return formula_.get();
}

/*
 * Convenience Functions
 */

// Return empirical formula for supplied Species
const char* EmpiricalFormula::formula(const Species* species, bool richText)
{
	static EmpiricalFormula formula;

	formula.reset();

	ListIterator<SpeciesAtom> atomIterator(species->atoms());
	while (SpeciesAtom* i = atomIterator.iterate()) formula.add(i->element());

	return (richText ? formula.richTextFormula() : formula.formula());
}

// Return empirical formula for supplied SpeciesAtom reflist
const char* EmpiricalFormula::formula(const RefList<SpeciesAtom>& atoms, bool richText)
{
	static EmpiricalFormula formula;

	formula.reset();

	for (SpeciesAtom* i : atoms) formula.add(i->element());

	return (richText ? formula.richTextFormula() : formula.formula());
}
