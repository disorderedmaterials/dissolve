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
#include "base/enumoptions.h"
#include "templates/reflist.h"

// Forward Declarations
class CoreData;
class ForcefieldAngleTerm;
class ForcefieldAtomType;
class ForcefieldBondTerm;
class ForcefieldTorsionTerm;
class Species;
class SpeciesAtom;

// Forcefield Base Class
class Forcefield : public Elements, public ListItem<Forcefield>
{
	public:
	// Constructor / Destructor
	Forcefield();
	virtual ~Forcefield();


	/*
	 * Definition
	 */
	public:
	// ShortRange Interaction Type
	enum ShortRangeType
	{
		UndefinedType,			/* Undefined short-range type */
		NoInteractionType,		/* No short-range dispersive forces */
		LennardJonesType,		/* Lennard-Jones 12-6 form with Lorentz-Berthelot combination rules */
		LennardJonesGeometricType,	/* Lennard-Jones 12-6 form with Geometric combination rules */
		nShortRangeTypes		/* Number of short-range interaction types */
	};
	// Return enum options for ShortRangeType
	static EnumOptions<ShortRangeType> shortRangeTypes();
	// Return name of Forcefield
	virtual const char* name() const = 0;
	// Return description of Forcefield
	virtual const char* description() const = 0;
	// Return short-range interaction style for AtomTypes
	virtual ShortRangeType shortRangeType() const = 0;


	/*
	 * Atom Type Data
	 */
	protected:
	// Atom type data, grouped by element
	Array< RefList<ForcefieldAtomType> > atomTypesByElementPrivate_;

	protected:
	// Determine and return atom type for specified SpeciesAtom
	virtual ForcefieldAtomType* determineAtomType(SpeciesAtom* i) const = 0;

	public:
	// Register specified atom type to given Element
	void registerAtomType(ForcefieldAtomType* atomType, int Z);
	// Return the named ForcefieldAtomType (if it exists)
	ForcefieldAtomType* atomTypeByName(const char* name, Element* element = NULL) const;


	/*
	 * Term Data
	 */
	private:
	// Bond terms of the Forcefield
	RefList<ForcefieldBondTerm> bondTerms_;
	// Angle terms of the Forcefield
	RefList<ForcefieldAngleTerm> angleTerms_;
	// Torsion terms of the Forcefield
	RefList<ForcefieldTorsionTerm> torsionTerms_;

	public:
	// Register specified bond term
	void registerBondTerm(ForcefieldBondTerm* bondTerm);
	// Return bond term for the supplied atom type pair (if it exists)
	ForcefieldBondTerm* bondTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j) const;
	// Register specified angle term
	void registerAngleTerm(ForcefieldAngleTerm* angleTerm);
	// Return angle term for the supplied atom type trio (if it exists)
	ForcefieldAngleTerm* angleTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k) const;
	// Register specified torsion term
	void registerTorsionTerm(ForcefieldTorsionTerm* torsionTerm);
	// Return torsion term for the supplied atom type quartet (if it exists)
	ForcefieldTorsionTerm* torsionTerm(const ForcefieldAtomType* i, const ForcefieldAtomType* j, const ForcefieldAtomType* k, const ForcefieldAtomType* l) const;


	/*
	 * Term Assignment
	 */
	public:
	// Assign suitable AtomTypes to the supplied Species
	virtual bool assignAtomTypes(Species* sp, CoreData& coreData, bool keepExisting = false) const = 0;
	// Assign intramolecular parameters to the supplied Species
	virtual bool assignIntramolecular(Species* sp, bool useExistingTypes, bool assignBonds, bool assignAngles, bool assignTorsions) const;


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
