/*
	*** Forcefield Data
	*** src/data/ff.h
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

#ifndef DISSOLVE_FORCEFIELD_H
#define DISSOLVE_FORCEFIELD_H

#include "data/elements.h"

// Forward Declarations
class CoreData;
class Species;
class SpeciesAtom;

// Forcefield Base Class
class Forcefield : public Elements
{
	/*
	 * Definition
	 */
	public:
	// Return name of Forcefield
	virtual const char* name() = 0;


	/*
	 * Term Generation
	 */
	public:
	// Create and assign suitable AtomTypes for the supplied Species
	virtual bool createAtomTypes(Species* sp, CoreData& coreData) = 0;
	// Generate missing bond terms for the supplied Species
	virtual bool generateBondTerms(Species* sp) = 0;
	// Generate missing angle terms for the supplied Species
	virtual bool generateAngleTerms(Species* sp) = 0;
	// Generate missing torsion terms for the supplied Species
	virtual bool generateTorsionTerms(Species* sp) = 0;


	/*
	 * Atom Environment Helpers
	 */
	public:
	// Atom Geometry enum
	enum AtomGeometry { UnboundGeometry, TerminalGeometry, LinearGeometry, TShapeGeometry, TrigonalPlanarGeometry, TetrahedralGeometry, SquarePlanarGeometry, TrigonalBipyramidalGeometry, OctahedralGeometry, nAtomGeometries };

	protected:
	// Calculate and return the geometry of the specified SpeciesAtom
	AtomGeometry geometryOfAtom(SpeciesAtom* i) const;
	// Return whether the specified SpeciesAtom exists in the specified geometry
	bool isAtomGeometry(SpeciesAtom* i, AtomGeometry geom) const;
	// Return whether supplied bonding pattern around the SpeciesAtom matches *exactly*
	bool isBondPattern(const SpeciesAtom* i, const int nSingle, const int nDouble = 0, const int nTriple = 0, const int nQuadruple = 0, const int nAromatic = 0) const;
	// Return whether the specified atom is bound to a specific element (and count thereof)
	bool isBoundTo(const SpeciesAtom* i, Element* element, const int count = 1, bool allowMoreThanCount = true) const;
	// Guess and return oxidation state for the specified SpeciesAtom
	int guessOxidationState(const SpeciesAtom* i) const;
};

#endif
